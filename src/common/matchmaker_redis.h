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

#ifndef __MATCHMAKER_REDIS_H__
#define __MATCHMAKER_REDIS_H__


#include <cpp_redis/cpp_redis>

#include "matchmaker.h"


namespace zmqproxy
{

    class MatchmakerRedis : public Matchmaker
    {
        const Configuration& _conf;
        cpp_redis::redis_client _redis;


    public:

        MatchmakerRedis(const Configuration& conf);

        virtual void register_publisher(const PublisherAddressT& host);
        virtual void unregister_publisher(const PublisherAddressT& host);

        virtual void register_router(const std::string& hostname);
        virtual void unregister_router(const std::string& hostname);

        virtual std::list<PublisherAddressT> get_publishers() const;
        virtual std::list<std::string> get_routers() const;


    private:

        void add_host(const std::string& key, const std::string& host);
        void remove_host(const std::string& key, const std::string& host);
    };
}


#endif
