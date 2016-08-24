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


#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <string>
#include <list>

#include <boost/log/trivial.hpp>

#define LOG BOOST_LOG_TRIVIAL


namespace zmqproxy
{
    class Configuration
    {
        bool m_usePubSub;
        int m_targetExpire;
        int m_targetUpdate;
        std::string m_host;

        std::string m_redisHost;
        int m_redisPort;
        std::string m_redisPassword;

        std::list<std::string> m_sentinelHosts;
        std::string m_sentinelGroupName;
        int m_sentinelWaitTimeout;
        int m_sentinelCheckTimeout;
        int m_sentinelSocketTimeout;

        unsigned int m_frontendPort;
        unsigned int m_backendPort;
        unsigned int m_publisherPort;

    public:
        Configuration(const std::string& configFile);
        bool usePubSub() const;
        int targetExpire() const;
        int targetUpdate() const;
        std::string host() const;

        std::string redisHost() const;
        unsigned int redisPort() const;
        std::string redisPassword() const;

        std::list<std::string> sentinelHosts() const;
        std::string sentinelGroupName() const;
        int sentinelWaitTimeout() const;
        int sentinelCheckTimeout() const;
        int sentinelSocketTimeout() const;

        void setFrontendPort(unsigned int port);
        void setBackendPort(unsigned int port);
        void setPublisherPort(unsigned int port);

        unsigned int getFrontendPort() const;
        unsigned int getBackendPort() const;
        unsigned int getPublisherPort() const;
    };
}


#endif
