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


#ifndef __NAMES_H__
#define __NAMES_H__


namespace zmqproxy
{
    enum class MessageType
    {
        Call    = 1,
        Cast    = 2,
        Fanout  = 3,
        Notify  = 4,
        Reply   = 5,
        Ack     = 6
    };


    inline bool is_multisend(MessageType msgType)
    {
        return msgType == MessageType::Fanout
                || msgType == MessageType::Notify;
    }


    inline bool is_direct(MessageType msgType)
    {
        return msgType == MessageType::Call
                || msgType == MessageType::Cast
                || msgType == MessageType::Reply;
    }
}

#endif
