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


#include <atomic>
#include <csignal>
#include <iostream>
#include <thread>
#include <boost/program_options.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "centralproxy.h"
#include "common/configuration.h"
#include "common/matchmaker_redis.h"


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


std::atomic<bool> quit(false);


void signal_handler(int sig)
{
    quit = true;
}


void init_signal_handling()
{
    struct sigaction sa;
    memset( &sa, 0, sizeof(sa) );
    sa.sa_handler = signal_handler;
    sigfillset(&sa.sa_mask);
    sigaction(SIGINT,&sa,NULL);
}


void init_logging()
{
    boost::log::core::get()->set_filter
    (
#ifdef DEBUG
        boost::log::trivial::severity >= boost::log::trivial::debug
#else
        boost::log::trivial::severity >= boost::log::trivial::info
#endif
    );
}


int main(int argc, char* argv[])
{
    init_signal_handling();
    init_logging();

    namespace po = boost::program_options;
    po::options_description desc("ZeroMQ proxy service");
    std::string configFile;
    unsigned int frontend_port = 0;
    unsigned int backend_port = 0;
    unsigned int publisher_port = 0;

    desc.add_options()
      ("help,h", "Show help")

      ("config-file,c", po::value<std::string>(&configFile),
                "ZeroMQ proxy configuration file")

      ("frontend-port,front", po::value<unsigned int>(&frontend_port),
                "Frontend ROUTER port. RPC clients connect to frontend")

      ("backend-port,back", po::value<unsigned int>(&backend_port),
                "Backend ROUTER port. RPC servers connect to backend.")

      ("publisher-port,pub", po::value<unsigned int>(&publisher_port),
                "Publisher port. Fanout messages go over this port if pub/sub is used.");

    po::variables_map vm;

    try
    {
        po::parsed_options parsed = po::command_line_parser(argc, argv)
                .options(desc).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);

        zmqproxy::Configuration config(configFile);

        if (frontend_port)
            config.set_frontend_port(frontend_port);

        if (backend_port)
            config.set_backend_port(backend_port);

        if (publisher_port)
            config.set_publisher_port(publisher_port);

        zmqproxy::CentralProxy proxy(config);

        while (true)
        {
            proxy.poll_for_messages();
            if( quit )
                // exit normally after SIGINT
                throw zmqproxy::SystemExit();
        }
    }
    catch(const zmqproxy::SystemExit& e)
    {
        LOG(error) << "Catched expected!" << e.what();
    }
    catch(const std::runtime_error& e)
    {
        LOG(error) << e.what();
    }
    catch(const std::exception& e)
    {
        LOG(error) << e.what();
    }
    catch(...)
    {
        LOG(error) << "Unexpected exception.";
    }

	return 0;
}
