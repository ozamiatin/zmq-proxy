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
#include "logging/logging.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/asio/ip/host_name.hpp>


using namespace zmqproxy;


Configuration::Configuration(const std::string& config_file)
    : _use_pub_sub(true),
      _target_expire(180),
      _target_update(120),
      _host(boost::asio::ip::host_name()),
      _redis_host("127.0.0.1"),
      _redis_port(6379),
      _sentinel_wait_timeout(5000),
      _sentinel_check_timeout(60000),
      _sentinel_socket_timeout(10000),
      _frontend_port(0),
      _backend_port(0),
      _publisher_port(0)
{
    if (!config_file.empty())
    {
        try
        {
            boost::property_tree::ptree conf;
            boost::property_tree::ini_parser::read_ini(config_file, conf);

            _use_pub_sub = conf.get<bool>("oslo_messaging_zmq.use_pub_sub", _use_pub_sub);
            _target_expire = conf.get<int>("oslo_messaging_zmq.zmq_target_expire", _target_expire);
            _target_update = conf.get<int>("oslo_messaging_zmq.zmq_target_update", _target_update);
            _host = conf.get<std::string>("oslo_messaging_zmq.rpc_zmq_host", _host);

            _redis_host = conf.get<std::string>("matchmaker_redis.host", _redis_host);
            _redis_port = conf.get<int>("matchmaker_redis.port", _redis_port);
            _redis_password = conf.get<std::string>("matchmaker_redis.password", _redis_password);

            LOG(info) << "Read configuration from '" << config_file << "'";
        }
        catch(const std::exception& e)
        {
            LOG(warning) << "Unrecognized configuration file '"
                         << config_file << "', used default configuration."
                         << e.what();
        }
    }
    LOG(info) << std::boolalpha << "use_pub_sub=" << _use_pub_sub;
    LOG(info) << "zmq_target_expire=" << _target_expire;
    LOG(info) << "zmq_target_update=" << _target_update;
    LOG(info) << "rpc_zmq_host=" << _host;
}


bool Configuration::use_pub_sub() const
{
    return _use_pub_sub;
}


int Configuration::target_expire() const
{
    return _target_expire;
}


int Configuration::target_update() const
{
    return _target_update;
}


std::string Configuration::host() const
{
    return _host;
}


std::string Configuration::redis_host() const
{
    return _redis_host;
}


unsigned int Configuration::redis_port() const
{
    return _redis_port;
}


std::string Configuration::redis_password() const
{
    return _redis_password;
}


std::list<std::string> Configuration::sentinel_hosts() const
{
    return _sentinel_hosts;
}


std::string Configuration::sentinel_group_name() const
{
    return _sentinel_group_name;
}


int Configuration::sentinel_wait_timeout() const
{
    return _sentinel_wait_timeout;
}


int Configuration::sentinel_check_timeout() const
{
    return _sentinel_check_timeout;
}


int Configuration::sentinel_socket_timeout() const
{
    return _sentinel_socket_timeout;
}


void Configuration::set_frontend_port(unsigned int port)
{
    _frontend_port = port;
}


void Configuration::set_backend_port(unsigned int port)
{
    _backend_port = port;
}


void Configuration::set_publisher_port(unsigned int port)
{
    _publisher_port = port;
}


unsigned int Configuration::get_frontend_port() const
{
    return _frontend_port;
}


unsigned int Configuration::get_backend_port() const
{
    return _backend_port;
}


unsigned int Configuration::get_publisher_port() const
{
    return _publisher_port;
}
