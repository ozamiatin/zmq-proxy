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


#include "matchmaker_redis.h"

#include "configuration.h"
#include "names.h"

#include <sstream>


using namespace zmqproxy;


MatchmakerRedis::MatchmakerRedis(const Configuration& conf)
    : m_conf(conf)
{
    LOG(info) << "Connecting to redis: "
              << "Host: " << conf.redisHost() << " "
              << "Port: " << conf.redisPort();
    m_redis.connect(conf.redisHost(), conf.redisPort());
}


void MatchmakerRedis::registerPublisher(const PublisherAddressT& host)
{
    addHost(PUBLISHERS_KEY, host.first + "," + host.second);
}


void MatchmakerRedis::unregisterPublisher(const PublisherAddressT& host)
{
    removeHost(PUBLISHERS_KEY, host.first + "," + host.second);
}


void MatchmakerRedis::registerRouter(const std::string& hostname)
{
    addHost(ROUTERS_KEY, hostname);
}


void MatchmakerRedis::unregisterRouter(const std::string& hostname)
{
    removeHost(ROUTERS_KEY, hostname);
}


std::list<PublisherAddressT> MatchmakerRedis::getPublishers() const
{
    return std::list<PublisherAddressT>();
}


std::list<std::string> MatchmakerRedis::getRouters() const
{
    return std::list<std::string>();
}


void MatchmakerRedis::addHost(const std::string& key, const std::string& host)
{
    if (!m_redis.is_connected())
        throw std::runtime_error("Redis server is not connected!");

    LOG(debug) << "Executing: " << "SADD " << key << " " << host;
    m_redis.send({"SADD", key, host}, [](const cpp_redis::reply& reply){
        if (reply.is_error())
            LOG(error) << "Command execution failed!";
    });

    if (m_conf.targetExpire() >= 0)
    {
        LOG(debug) << "Executing: " << "EXPIRE " << key;
        m_redis.send({"EXPIRE", key, std::to_string(m_conf.targetExpire())},
                     [](const cpp_redis::reply& reply){
            if (reply.is_error())
                LOG(error) << "Command execution failed!";
        });
    }
}


void MatchmakerRedis::removeHost(const std::string& key, const std::string& host)
{
    if (!m_redis.is_connected())
        throw std::runtime_error("Redis server is not connected!");

    LOG(debug) << "Executing: " << "SREM " << key << " " << host;
    m_redis.send({"SREM", key, host}, [](const cpp_redis::reply& reply){
        if (reply.is_error())
            LOG(error) << "Command execution failed!";
    });
}
