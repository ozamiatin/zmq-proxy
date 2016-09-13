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
    : _conf(conf)
{
    LOG(info) << "Connecting to redis: "
              << "Host: " << conf.redis_host() << " "
              << "Port: " << conf.redis_port();
    _redis.connect(conf.redis_host(), conf.redis_port());
    LOG(debug) << "After redis connected";
}


void MatchmakerRedis::register_publisher(const PublisherAddressT& host)
{
    add_host(PUBLISHERS_KEY, host.first + "," + host.second);
}


void MatchmakerRedis::unregister_publisher(const PublisherAddressT& host)
{
    remove_host(PUBLISHERS_KEY, host.first + "," + host.second);
}


void MatchmakerRedis::register_router(const std::string& hostname)
{
    add_host(ROUTERS_KEY, hostname);
}


void MatchmakerRedis::unregister_router(const std::string& hostname)
{
    remove_host(ROUTERS_KEY, hostname);
}


std::list<PublisherAddressT> MatchmakerRedis::get_publishers() const
{
    return std::list<PublisherAddressT>();
}


std::list<std::string> MatchmakerRedis::get_routers() const
{
    return std::list<std::string>();
}


void MatchmakerRedis::add_host(const std::string& key, const std::string& host)
{
    if (!_redis.is_connected())
        throw std::runtime_error("Redis server is not connected!");

    LOG(debug) << "Executing: " << "SADD " << key << " " << host;
    _redis.send({"SADD", key, host}, [](const cpp_redis::reply& reply){
        if (reply.is_error())
            LOG(error) << "Command execution failed!";
    });

    if (_conf.target_expire() >= 0)
    {
        LOG(debug) << "Executing: " << "EXPIRE " << key;
        _redis.send({"EXPIRE", key, std::to_string(_conf.target_expire())},
                     [](const cpp_redis::reply& reply){
            if (reply.is_error())
                LOG(error) << "Command execution failed!";
        });
    }
}


void MatchmakerRedis::remove_host(const std::string& key, const std::string& host)
{
    if (!_redis.is_connected())
        throw std::runtime_error("Redis server is not connected!");

    LOG(debug) << "Executing: " << "SREM " << key << " " << host;
    _redis.send({"SREM", key, host}, [](const cpp_redis::reply& reply){
        if (reply.is_error())
            LOG(error) << "Command execution failed!";
    });
}
