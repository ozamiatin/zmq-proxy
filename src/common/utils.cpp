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


#include "utils.h"


namespace zmqproxy
{


std::string toString(MessageType msgType)
{
    switch(msgType)
    {
        case MessageType::Call:
            return "Call";
        case MessageType::Cast:
            return "Cast";
        case MessageType::Fanout:
            return "Fanout";
        case MessageType::Notify:
            return "Notify";
        case MessageType::Reply:
            return "Reply";
        case MessageType::Ack:
            return "Ack";
        default:
            break;
    }

    throw std::logic_error("Can not convert to a string, unknown MessageType!");
}


std::string getString(const zmq::message_t& part)
{
    return std::string(static_cast<const char*>(part.data()), part.size());
}


int getInteger(const zmq::message_t& part)
{
    return atoi(getString(part).c_str());
}


MessageType getMessageType(const zmq::message_t& part)
{
    return static_cast<MessageType>(getInteger(part));
}


bool isMultisend(MessageType msgType)
{
    return msgType == MessageType::Fanout
            || msgType == MessageType::Notify;
}


bool isDirect(MessageType msgType)
{
    return msgType == MessageType::Call
            || msgType == MessageType::Cast
            || msgType == MessageType::Reply;
}


void sendString(zmq::socket_t& socket, const std::string& string, int flags)
{
    zmq::message_t message(string.size());
    memcpy (message.data(), string.data(), string.size());

    bool rc = socket.send (message, flags);
    if (!rc)
        throw zmq::error_t();
}

}
