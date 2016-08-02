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

        virtual void registerPublisher(const PublisherAddressT& host) = 0;
        virtual void unregisterPublisher(const PublisherAddressT& host) = 0;

        virtual void registerRouter(const std::string& hostname) = 0;
        virtual void unregisterRouter(const std::string& hostname) = 0;

        virtual std::list<PublisherAddressT> getPublishers() const = 0;
        virtual std::list<std::string> getRouters() const = 0;

    };
}

#endif
