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


namespace zmqproxy
{
    class Configuration
    {
        bool _use_pub_sub;
        int _target_expire;
        int _target_update;
        std::string _host;

        std::string _redis_host;
        int _redis_port;
        std::string _redis_password;

        std::list<std::string> _sentinel_hosts;
        std::string _sentinel_group_name;
        int _sentinel_wait_timeout;
        int _sentinel_check_timeout;
        int _sentinel_socket_timeout;

        unsigned int _frontend_port;
        unsigned int _backend_port;
        unsigned int _publisher_port;

        bool _ack_pub_sub;

    public:
        Configuration(const std::string& config_file);
        bool use_pub_sub() const;
        int target_expire() const;
        int target_update() const;
        std::string host() const;

        std::string redis_host() const;
        unsigned int redis_port() const;
        std::string redis_password() const;

        std::list<std::string> sentinel_hosts() const;
        std::string sentinel_group_name() const;
        int sentinel_wait_timeout() const;
        int sentinel_check_timeout() const;
        int sentinel_socket_timeout() const;

        void set_frontend_port(unsigned int port);
        void set_backend_port(unsigned int port);
        void set_publisher_port(unsigned int port);

        void set_ack_pub_sub(bool ack);

        unsigned int get_frontend_port() const;
        unsigned int get_backend_port() const;
        unsigned int get_publisher_port() const;

        bool get_ack_pub_sub() const;
    };
}


#endif
