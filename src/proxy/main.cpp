/**
 *   (C) Copyright 2016 Mirantis, Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License"); you may
 *   not use this file except in compliance with the License. You may obtain
 *   a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *   WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *   License for the specific language governing permissions and limitations
 *   under the License.
 */


#include <csetjmp>
#include <csignal>
#include <iostream>
#include <thread>
#include <easylogging++.h>
#include <boost/program_options.hpp>

#include "centralproxy.h"
#include "common/configuration.h"
#include "common/matchmaker_redis.h"


INITIALIZE_EASYLOGGINGPP


namespace zmqproxy
{
    class SystemExit : public std::runtime_error
    {
    public:
        SystemExit()
            : std::runtime_error("Program terminated by SIGINT")
        {

        }
    };
}


std::jmp_buf gJumpBuffer;


void signal_handler(int sig)
{
    std::longjmp(gJumpBuffer, sig);
}


void init_signal_handling()
{
    std::signal(SIGINT, signal_handler);
    std::signal(SIGKILL, signal_handler);
    std::signal(SIGABRT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    int sig = 0;
    if ((sig = setjmp(gJumpBuffer)) != 0)
    {
        throw zmqproxy::SystemExit();
    }
}


int main(int argc, char* argv[])
{
    init_signal_handling();
    START_EASYLOGGINGPP(argc, argv);
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format,
                                       "%level %datetime{%H:%m:%s} (%func): %msg");
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);

    namespace po = boost::program_options;
    po::options_description desc("ZeroMQ proxy service");
    std::string configFile;
    unsigned int frontendPort = 0;
    unsigned int backendPort = 0;
    unsigned int publisherPort = 0;

    desc.add_options()
      ("help,h", "Show help")

      ("config-file,c", po::value<std::string>(&configFile),
                "ZeroMQ proxy configuration file")

      ("frontend-port,fe", po::value<unsigned int>(&frontendPort),
                "Frontend ROUTER port. RPC clients connect to frontend")

      ("backend-port,be", po::value<unsigned int>(&backendPort),
                "Backend ROUTER port. RPC servers connect to backend.")

      ("publisher-port,pub", po::value<unsigned int>(&publisherPort),
                "Publisher port. Fanout messages go over this port if pub/sub is used.");

    po::variables_map vm;

    try
    {
        po::parsed_options parsed = po::command_line_parser(argc, argv)
                .options(desc).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);

        zmqproxy::Configuration config(configFile);

        if (frontendPort)
            config.setFrontendPort(frontendPort);

        if (backendPort)
            config.setBackendPort(backendPort);

        if (publisherPort)
            config.setPublisherPort(publisherPort);

        zmqproxy::MatchmakerRedis matchmakerRedis(config);
        zmqproxy::CentralProxy proxy(config, matchmakerRedis);

        while (true)
        {
            proxy.pollForMessages();
        }
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << e.what();
    }

	return 0;
}
