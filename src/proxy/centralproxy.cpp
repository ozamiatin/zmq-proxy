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

#include <zmq_addon.hpp>

#include "logging/logging.h"
#include "common/configuration.h"
#include "common/matchmaker.h"
#include "common/utils.h"


using namespace zmqproxy;


const std::string EMPTY = "";

const int REPLY_ID_IDX = 0;
const int EMPTY_IDX = 1;
const int MESSAGE_TYPE_IDX = 2;
const int ROUTING_KEY_IDX = 3;
const int MESSAGE_ID_IDX = 4;


CentralProxy::CentralProxy(const Configuration& conf)
    : _conf(conf),
      _matchmaker(Matchmaker::create(conf)),
      _context(zmq::context_t(3)),
      _fe_router(zmq::socket_t(_context, zmq::socket_type::router)),
      _be_router(zmq::socket_t(_context, zmq::socket_type::router)),
      _publisher(zmq::socket_t(_context, zmq::socket_type::pub)),
      _fe_router_address(conf.host() + ":" + std::to_string(conf.get_frontend_port())),
      _be_router_address(conf.host() + ":" + std::to_string(conf.get_backend_port())),
      _publisher_address(conf.host() + ":" + std::to_string(conf.get_publisher_port()))
{
    LOG(info) << "Starting central router ";

    LOG(info) << "Bind to address: " << "tcp://*:" + std::to_string(conf.get_frontend_port());
    _fe_router.bind("tcp://*:" + std::to_string(conf.get_frontend_port()));
    LOG(info) << "Bind to address: " << "tcp://*:" + std::to_string(conf.get_backend_port());
    _be_router.bind("tcp://*:" + std::to_string(conf.get_backend_port()));
    LOG(info) << "Bind to address: " << "tcp://*:" + std::to_string(conf.get_publisher_port());
    _publisher.bind("tcp://*:" + std::to_string(conf.get_publisher_port()));

    _stop_updates.store(false);
    LOG(debug) << "Before updater created";
    _matchmaker_updater.reset(new std::thread(&CentralProxy::update_matchmaker, this));

    LOG(debug) << "Updater created";
}


void CentralProxy::update_matchmaker()
{
    LOG(debug) << "Entered update_matchmaker first time!";
    while (!_stop_updates)
    {
        _matchmaker->register_publisher(std::make_pair(_publisher_address, _fe_router_address));
        _matchmaker->register_router(_be_router_address);

        LOG(info) << "[PUB:" << std::to_string(_conf.get_publisher_port())
                  << ", ROUTER:"
                  << std::to_string(_conf.get_frontend_port()) << "] Update PUB publisher";

        LOG(info) << "[Backend ROUTER:"
                  << std::to_string(_conf.get_publisher_port())
                  << "] Update ROUTER";

        std::this_thread::sleep_for(std::chrono::seconds(_conf.target_update()));
    }
}


CentralProxy::~CentralProxy()
{
    _stop_updates.store(true);

    LOG(info) << "Destroy central router ";
    _matchmaker->unregister_publisher(std::make_pair(_publisher_address, _fe_router_address));
    _matchmaker->unregister_router(_be_router_address);
}


void CentralProxy::poll_for_messages()
{
    LOG(debug) << "Poll for messages";

    //  Initialize poll set
    zmq::pollitem_t items [] = {
        { static_cast<void *>(_fe_router), 0, ZMQ_POLLIN, 0 },
        { static_cast<void *>(_be_router), 0, ZMQ_POLLIN, 0 }
    };
    //  Process messages from both sockets
    while (1) {
        zmq::poll (&items [0], 2, -1);
        if (items [0].revents & ZMQ_POLLIN) {
            redirect_in_request(_fe_router, _be_router);
        }
        if (items [1].revents & ZMQ_POLLIN) {
            redirect_in_request(_be_router, _fe_router);
        }
    }
}


void CentralProxy::redirect_in_request(zmq::socket_t& socket_fe, zmq::socket_t& socket_be)
{
    auto message = receive_multipart(socket_fe);

    if (message.size() < 3)
    {
        LOG(error) << "Received message with wrong format! Message skipped ...";
        return;
    }

    auto message_type = get_message_type(message[MESSAGE_TYPE_IDX]);
    auto& message_id = message[MESSAGE_ID_IDX];
    auto& reply_id = message[REPLY_ID_IDX];
    auto& routing_key = message[ROUTING_KEY_IDX];

    if (is_direct(message_type))
    {
        LOG(debug) << "Dispatching "
                   << to_string(message_type)
                   << " message "
                   << get_string(message_id)
                   << " - from "
                   << get_string(reply_id)
                   << " to -> "
                   << get_string(routing_key);

        socket_be.send(routing_key, ZMQ_SNDMORE);
        send_string(socket_be, EMPTY, ZMQ_SNDMORE);
        socket_be.send(reply_id, ZMQ_SNDMORE);
        socket_be.send(message[MESSAGE_TYPE_IDX], ZMQ_SNDMORE);
        dispatch_message_tail(socket_be, message.begin() + MESSAGE_ID_IDX, message.end());
    }
    else if (is_multisend(message_type))
    {
        LOG(debug) << "Publishing message "
                   << get_string(message_id)
                   << " on [" << get_string(routing_key) << "]";

        _publisher.send(routing_key, ZMQ_SNDMORE);
        dispatch_message_tail(_publisher, message.begin() + MESSAGE_ID_IDX, message.end());
    }
}


std::vector<zmq::message_t> CentralProxy::receive_multipart(zmq::socket_t& socket_fe)
{
    std::vector<zmq::message_t> message;
    bool more = true;
    while (more)
    {
        message.push_back(zmq::message_t());
        zmq::message_t& msg = message.back();
        socket_fe.recv(&msg);
        more = msg.more();
    }
    return message;
}


template <typename FwdIt>
void CentralProxy::dispatch_message_tail(zmq::socket_t& socket_be, FwdIt begin, FwdIt end)
{
    std::for_each(begin, end, [&](zmq::message_t& msg){
        socket_be.send(msg, msg.more() ? ZMQ_SNDMORE : 0);
    });
}
