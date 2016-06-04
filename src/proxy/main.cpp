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


#include <iostream>
#include <thread>
#include <easylogging++.h>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "centralproxy.h"


INITIALIZE_EASYLOGGINGPP


int main(int argc, char* argv[])
{
    START_EASYLOGGINGPP(argc, argv);
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%level %datetime{%H:%m:%s} (%func): %msg");
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);


    try
    {
        boost::property_tree::ptree conf;
        boost::property_tree::ini_parser::read_ini("/Users/ozamiatin/zmq.conf", conf);

        LOG(INFO) << std::boolalpha << "use_pub_sub=" << conf.get<bool>("DEFAULT.use_pub_sub");

        zmqproxy::CentralProxy proxy("tcp://*:19001");

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
