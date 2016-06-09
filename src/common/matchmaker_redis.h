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
        const Configuration& m_conf;
        cpp_redis::redis_client m_redis;


    public:

        MatchmakerRedis(const Configuration& conf);

        virtual void registerPublisher(const PublisherAddressT& host);
        virtual void unregisterPublisher(const PublisherAddressT& host);

        virtual void registerRouter(const std::string& hostname);
        virtual void unregisterRouter(const std::string& hostname);

        virtual std::list<PublisherAddressT> getPublishers() const;
        virtual std::list<std::string> getRouters() const;


    private:

        void addKey(const std::string& key);
        void removeKey(const std::string& key);
    };
}


#endif
