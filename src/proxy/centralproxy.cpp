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


#include <easylogging++.h>
#include <zmq_addon.hpp>

#include "common/configuration.h"
#include "centralproxy.h"
#include "common/matchmaker.h"


using namespace zmqproxy;


CentralProxy::CentralProxy(const Configuration& conf, Matchmaker& matchmaker)
    : m_conf(conf),
      m_matchmaker(matchmaker),
      m_context(zmq::context_t(3)),
      m_feRouter(zmq::socket_t(m_context, zmq::socket_type::router)),
      m_beRouter(zmq::socket_t(m_context, zmq::socket_type::router)),
      m_publisher(zmq::socket_t(m_context, zmq::socket_type::pub)),
      m_fePort(35001),
      m_bePort(35002),
      m_publisherPort(35003),
      m_feRouterAddress(conf.host() + ":" + std::to_string(m_fePort)),
      m_beRouterAddress(conf.host() + ":" + std::to_string(m_bePort)),
      m_publisherAddress(conf.host() + ":" + std::to_string(m_publisherPort))
{
    el::Loggers::getLogger("CentralProxy");
    CLOG(INFO, "CentralProxy") << "Starting central router ";

    CLOG(INFO, "CentralProxy") << "Bind to address: " << "tcp://*:" + std::to_string(m_fePort);
    m_feRouter.bind("tcp://*:" + std::to_string(m_fePort));
    CLOG(INFO, "CentralProxy") << "Bind to address: " << "tcp://*:" + std::to_string(m_bePort);
    m_beRouter.bind("tcp://*:" + std::to_string(m_bePort));
    CLOG(INFO, "CentralProxy") << "Bind to address: " << "tcp://*:" + std::to_string(m_publisherPort);
    m_publisher.bind("tcp://*:" + std::to_string(m_publisherPort));

    m_matchmaker.registerPublisher(std::make_pair(m_publisherAddress, m_feRouterAddress));
    m_matchmaker.registerRouter(m_beRouterAddress);
}


CentralProxy::~CentralProxy()
{
    CLOG(INFO, "CentralProxy") << "Destroy central router ";
    m_matchmaker.unregisterPublisher(std::make_pair(m_publisherAddress, m_feRouterAddress));
    m_matchmaker.unregisterRouter(m_beRouterAddress);
}


void CentralProxy::pollForMessages()
{
//    void* feSocket = *m_feRouter;
//    void* beSocket = *m_beRouter;

    //  Initialize poll set
    zmq::pollitem_t items [] = {
        { static_cast<void *>(m_feRouter), 0, ZMQ_POLLIN, 0 },
        { static_cast<void *>(m_feRouter), 0, ZMQ_POLLIN, 0 }
    };
    //  Process messages from both sockets
    while (1) {
        CLOG(INFO, "CentralProxy") << "Start polling messages";
        zmq::poll (&items [0], 2, -1);

        if (items [0].revents & ZMQ_POLLIN) {
            receiveInRequest(m_feRouter);
        }
        if (items [1].revents & ZMQ_POLLIN) {
            //m_beRouter.recv(&message);
            //  Process weather update
        }
    }
}


void CentralProxy::receiveInRequest(zmq::socket_t& socket)
{
    CLOG(INFO, "CentralProxy") << "Message pending ";
    zmq::multipart_t message;
    message.recv(socket);
    CLOG(INFO, "CentralProxy") << "Received: " << message.str();
}
