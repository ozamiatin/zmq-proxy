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

    class CentralProxy
    {
        zmq::context_t m_context;
        zmq::socket_t m_fe_router;
        zmq::socket_t m_be_router;
        zmq::socket_t m_publisher;
        const std::string m_bind_address;

    public:

        CentralProxy(const std::string& bind_address);
        void pollForMessages();
    };
}

#endif
