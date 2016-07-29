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
#include <zmq_addon.hpp>

#include "common/configuration.h"
#include "common/matchmaker.h"
#include "common/utils.h"


using namespace zmqproxy;


const std::string EMPTY = "";


CentralProxy::CentralProxy(const Configuration& conf)
    : m_conf(conf),
      m_matchmaker(Matchmaker::create(conf)),
      m_context(zmq::context_t(3)),
      m_feRouter(zmq::socket_t(m_context, zmq::socket_type::router)),
      m_beRouter(zmq::socket_t(m_context, zmq::socket_type::router)),
      m_publisher(zmq::socket_t(m_context, zmq::socket_type::pub)),
      m_feRouterAddress(conf.host() + ":" + std::to_string(conf.getFrontendPort())),
      m_beRouterAddress(conf.host() + ":" + std::to_string(conf.getBackendPort())),
      m_publisherAddress(conf.host() + ":" + std::to_string(conf.getPublisherPort()))
{
    LOG(INFO) << "Starting central router ";

    LOG(INFO) << "Bind to address: " << "tcp://*:" + std::to_string(conf.getFrontendPort());
    m_feRouter.bind("tcp://*:" + std::to_string(conf.getFrontendPort()));
    LOG(INFO) << "Bind to address: " << "tcp://*:" + std::to_string(conf.getBackendPort());
    m_beRouter.bind("tcp://*:" + std::to_string(conf.getBackendPort()));
    LOG(INFO) << "Bind to address: " << "tcp://*:" + std::to_string(conf.getPublisherPort());
    m_publisher.bind("tcp://*:" + std::to_string(conf.getPublisherPort()));

    m_stopUpdates.store(false);
    m_matchmakerUpdater.reset(new std::thread(&CentralProxy::updateMatchmaker, this));
}


void CentralProxy::updateMatchmaker()
{
    while (!m_stopUpdates)
    {
        m_matchmaker->registerPublisher(std::make_pair(m_publisherAddress, m_feRouterAddress));
        m_matchmaker->registerRouter(m_beRouterAddress);

        LOG(INFO) << "[PUB:" << std::to_string(m_conf.getPublisherPort())
                                   << ", ROUTER:"
                                   << std::to_string(m_conf.getFrontendPort()) << "] Update PUB publisher";

        LOG(INFO) << "[Backend ROUTER:"
                                   << std::to_string(m_conf.getPublisherPort())
                                   << "] Update ROUTER";

        std::this_thread::sleep_for(std::chrono::seconds(m_conf.targetUpdate()));
    }
}


CentralProxy::~CentralProxy()
{
    m_stopUpdates.store(true);

    LOG(INFO) << "Destroy central router ";
    m_matchmaker->unregisterPublisher(std::make_pair(m_publisherAddress, m_feRouterAddress));
    m_matchmaker->unregisterRouter(m_beRouterAddress);
}


void CentralProxy::pollForMessages()
{
    //  Initialize poll set
    zmq::pollitem_t items [] = {
        { static_cast<void *>(m_feRouter), 0, ZMQ_POLLIN, 0 },
        { static_cast<void *>(m_beRouter), 0, ZMQ_POLLIN, 0 }
    };
    //  Process messages from both sockets
    while (1) {
        zmq::poll (&items [0], 2, -1);
        if (items [0].revents & ZMQ_POLLIN) {
            redirectInRequest(m_feRouter, m_beRouter);
        }
        if (items [1].revents & ZMQ_POLLIN) {
            redirectInRequest(m_beRouter, m_feRouter);
        }
    }
}


void CentralProxy::redirectInRequest(zmq::socket_t& socketFe, zmq::socket_t& socketBe)
{
    zmq::message_t replyId;
    socketFe.recv(&replyId);

    zmq::message_t empty;
    socketFe.recv(&empty);

    zmq::message_t msgType;
    socketFe.recv(&msgType);
    auto messageType = getMessageType(msgType);

    zmq::message_t routingKey;
    socketFe.recv(&routingKey);

    zmq::message_t messageId;
    socketFe.recv(&messageId);

    if (isDirect(messageType))
    {
        LOG(DEBUG) << "Dispatching "
                                    << toString(messageType)
                                    << " message "
                                    << getString(messageId)
                                    << " - to "
                                    << getString(routingKey);

        socketBe.send(routingKey, ZMQ_SNDMORE | ZMQ_NOBLOCK);

        sendString(socketBe, EMPTY, ZMQ_SNDMORE | ZMQ_NOBLOCK);
        socketBe.send(replyId, ZMQ_SNDMORE | ZMQ_NOBLOCK);
        socketBe.send(msgType, ZMQ_SNDMORE | ZMQ_NOBLOCK);
        socketBe.send(messageId, ZMQ_SNDMORE | ZMQ_NOBLOCK);
        dispatchMessageTail(socketFe, socketBe);
    }
    else if (isMultisend(messageType))
    {
        LOG(DEBUG) << "Publishing message "
                                    << getString(messageId)
                                    << " on [" << getString(routingKey) << "]";

        m_publisher.send(routingKey, ZMQ_SNDMORE | ZMQ_NOBLOCK);
        m_publisher.send(messageId, ZMQ_SNDMORE | ZMQ_NOBLOCK);
        dispatchMessageTail(socketFe, m_publisher);
    }
}


void CentralProxy::dispatchMessageTail(zmq::socket_t& socketFe, zmq::socket_t& socketBe)
{
    bool more = true;
    zmq::message_t msg;
    while (more)
    {
        socketFe.recv(&msg);
        more = msg.more();
        socketBe.send(msg, more ? ZMQ_SNDMORE | ZMQ_NOBLOCK : ZMQ_NOBLOCK);
    }
}
