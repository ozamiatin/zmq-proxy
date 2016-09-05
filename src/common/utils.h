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


#ifndef __ZMQ_PROXY_UTILS_HPP__
#define __ZMQ_PROXY_UTILS_HPP__


#include "zmq.hpp"
#include "names.h"


namespace zmqproxy
{

    std::string get_string(const zmq::message_t& part);
    int get_integer(const zmq::message_t& part);
    MessageType get_message_type(const zmq::message_t& part);
    std::string to_string(MessageType msgType);
    bool is_multisend(MessageType msgType);
    bool is_direct(MessageType msgType);

    void send_string(zmq::socket_t& socket, const std::string& string, int flags = 0);

}

#endif
