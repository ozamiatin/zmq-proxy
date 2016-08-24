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

#include "configuration.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/asio/ip/host_name.hpp>


using namespace zmqproxy;


Configuration::Configuration(const std::string& configFile)
    : m_usePubSub(true),
      m_targetExpire(180),
      m_targetUpdate(120),
      m_host(boost::asio::ip::host_name()),
      m_redisHost("127.0.0.1"),
      m_redisPort(6379),
      m_sentinelWaitTimeout(5000),
      m_sentinelCheckTimeout(60000),
      m_sentinelSocketTimeout(10000),
      m_frontendPort(0),
      m_backendPort(0),
      m_publisherPort(0)
{
    if (!configFile.empty())
    {
        try
        {
            boost::property_tree::ptree conf;
            boost::property_tree::ini_parser::read_ini(configFile, conf);

            m_usePubSub = conf.get<bool>("DEFAULT.use_pub_sub", m_usePubSub);
            m_targetExpire = conf.get<int>("DEFAULT.zmq_target_expire", m_targetExpire);
            m_targetUpdate = conf.get<int>("DEFAULT.zmq_target_update", m_targetUpdate);
            m_host = conf.get<std::string>("DEFAULT.rpc_zmq_host", m_host);

            m_redisHost = conf.get<std::string>("matchmaker_redis.host", m_redisHost);
            m_redisPort = conf.get<int>("matchmaker_redis.port", m_redisPort);
            m_redisPassword = conf.get<std::string>("matchmaker_redis.password", m_redisPassword);

            LOG(info) << "Read configuration from '" << configFile << "'";
        }
        catch(const std::exception& e)
        {
            LOG(warning) << "Unrecognized configuration file '"
                         << configFile << "', used default configuration."
                         << e.what();
        }
    }
    LOG(info) << std::boolalpha << "use_pub_sub=" << m_usePubSub;
    LOG(info) << "zmq_target_expire=" << m_targetExpire;
    LOG(info) << "zmq_target_update=" << m_targetUpdate;
    LOG(info) << "rpc_zmq_host=" << m_host;
}


bool Configuration::usePubSub() const
{
    return m_usePubSub;
}


int Configuration::targetExpire() const
{
    return m_targetExpire;
}


int Configuration::targetUpdate() const
{
    return m_targetUpdate;
}


std::string Configuration::host() const
{
    return m_host;
}


std::string Configuration::redisHost() const
{
    return m_redisHost;
}


unsigned int Configuration::redisPort() const
{
    return m_redisPort;
}


std::string Configuration::redisPassword() const
{
    return m_redisPassword;
}


std::list<std::string> Configuration::sentinelHosts() const
{
    return m_sentinelHosts;
}


std::string Configuration::sentinelGroupName() const
{
    return m_sentinelGroupName;
}


int Configuration::sentinelWaitTimeout() const
{
    return m_sentinelWaitTimeout;
}


int Configuration::sentinelCheckTimeout() const
{
    return m_sentinelCheckTimeout;
}


int Configuration::sentinelSocketTimeout() const
{
    return m_sentinelSocketTimeout;
}


void Configuration::setFrontendPort(unsigned int port)
{
    m_frontendPort = port;
}


void Configuration::setBackendPort(unsigned int port)
{
    m_backendPort = port;
}


void Configuration::setPublisherPort(unsigned int port)
{
    m_publisherPort = port;
}


unsigned int Configuration::getFrontendPort() const
{
    return m_frontendPort;
}


unsigned int Configuration::getBackendPort() const
{
    return m_backendPort;
}


unsigned int Configuration::getPublisherPort() const
{
    return m_publisherPort;
}
