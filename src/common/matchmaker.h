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


#ifndef __MATCHMAKER_H__
#define __MATCHMAKER_H__

#include <utility>
#include <string>
#include <list>
#include <memory>


namespace zmqproxy
{
    class Configuration;
    typedef std::pair<std::string, std::string> PublisherAddressT;


    class Matchmaker
    {
    public:
        virtual ~Matchmaker() {}

        static std::shared_ptr<Matchmaker> create(const Configuration& config);

        virtual void register_publisher(const PublisherAddressT& host) = 0;
        virtual void unregister_publisher(const PublisherAddressT& host) = 0;

        virtual void register_router(const std::string& hostname) = 0;
        virtual void unregister_router(const std::string& hostname) = 0;

        virtual std::list<PublisherAddressT> get_publishers() const = 0;
        virtual std::list<std::string> get_routers() const = 0;

    };
}

#endif
