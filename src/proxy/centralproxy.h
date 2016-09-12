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
#include <thread>
#include <atomic>


namespace zmqproxy
{
    class Configuration;
    class Matchmaker;

    class CentralProxy
    {
        const Configuration& _conf;
        std::shared_ptr<Matchmaker> _matchmaker;
        zmq::context_t _context;
        zmq::socket_t _fe_router;
        zmq::socket_t _be_router;
        zmq::socket_t _publisher;

        std::atomic<bool> _stop_updates;
        std::unique_ptr<std::thread> _matchmaker_updater;

        std::string _fe_router_address;
        std::string _be_router_address;
        std::string _publisher_address;


    public:
        CentralProxy(const Configuration& conf);
        ~CentralProxy();
        void poll_for_messages();

    private:
        void update_matchmaker();
        void redirect_in_request(zmq::socket_t& socket_fe, zmq::socket_t& socket_be);
        void dispatch_message_tail(zmq::socket_t& socket_fe, zmq::socket_t& socket_be);

        template <typename FwdIt>
            void dispatch_message_tail(zmq::socket_t& socket_be, FwdIt begin, FwdIt end);

        std::vector<zmq::message_t> receive_multipart(zmq::socket_t& socket_fe);
    };
}

#endif
