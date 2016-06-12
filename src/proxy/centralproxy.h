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


#ifndef __CENTRALPROXY_H__
#define __CENTRALPROXY_H__


#include "zmq.hpp"


namespace zmqproxy
{
    class Configuration;
    class Matchmaker;

    class CentralProxy
    {
        const Configuration& m_conf;
        Matchmaker& m_matchmaker;
        zmq::context_t m_context;
        zmq::socket_t m_feRouter;
        zmq::socket_t m_beRouter;
        zmq::socket_t m_publisher;

        unsigned int m_fePort;
        unsigned int m_bePort;
        unsigned int m_publisherPort;
        std::string m_feRouterAddress;
        std::string m_beRouterAddress;
        std::string m_publisherAddress;


    public:

        CentralProxy(const Configuration& conf, Matchmaker& matchmaker);
        ~CentralProxy();
        void pollForMessages();
        void receiveInRequest(zmq::socket_t& socket);
    };
}

#endif
