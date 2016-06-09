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


#include "centralproxy.h"
#include <easylogging++.h>


using namespace zmqproxy;


CentralProxy::CentralProxy(const Configuration& conf)
    : m_conf(conf),
      m_context(zmq::context_t(3)),
      m_fe_router(zmq::socket_t(m_context, zmq::socket_type::router)),
      m_be_router(zmq::socket_t(m_context, zmq::socket_type::router)),
      m_publisher(zmq::socket_t(m_context, zmq::socket_type::pub))
{
    el::Loggers::getLogger("CentralProxy");
    CLOG(INFO, "CentralProxy") << "Starting central router ";
}


void CentralProxy::pollForMessages()
{

}
