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
        const Configuration& m_conf;
        std::shared_ptr<Matchmaker> m_matchmaker;
        zmq::context_t m_context;
        zmq::socket_t m_feRouter;
        zmq::socket_t m_beRouter;
        zmq::socket_t m_publisher;

        std::atomic<bool> m_stopUpdates;
        std::unique_ptr<std::thread> m_matchmakerUpdater;

        std::string m_feRouterAddress;
        std::string m_beRouterAddress;
        std::string m_publisherAddress;


    public:
        CentralProxy(const Configuration& conf);
        ~CentralProxy();
        void pollForMessages();

    private:
        void updateMatchmaker();
        void redirectInRequest(zmq::socket_t& socketFe, zmq::socket_t& socketBe);
        void dispatchMessageTail(zmq::socket_t& socketFe, zmq::socket_t& socketBe);
    };
}

#endif
