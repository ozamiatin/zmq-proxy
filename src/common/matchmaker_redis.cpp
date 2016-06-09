#include "matchmaker_redis.h"

#include <sstream>
#include <easylogging++.h>


#include "configuration.h"


using namespace zmqproxy;


MatchmakerRedis::MatchmakerRedis(const Configuration& conf)
    : m_conf(conf)
{
    el::Loggers::getLogger("MatchmakerRedis");
    CLOG(INFO, "MatchmakerRedis") << "Connecting to redis: "
                                  << "Host: " << conf.redisHost() << " "
                                  << "Port: " << conf.redisPort();
    m_redis.connect(conf.redisHost(), conf.redisPort());
}


void MatchmakerRedis::registerPublisher(const PublisherAddressT& host)
{
    addKey(host.first + "," + host.second);
}


void MatchmakerRedis::unregisterPublisher(const PublisherAddressT& host)
{
    removeKey(host.first + "," + host.second);
}


void MatchmakerRedis::registerRouter(const std::string& hostname)
{
    addKey(hostname);
}


void MatchmakerRedis::unregisterRouter(const std::string& hostname)
{
    removeKey(hostname);
}


std::list<PublisherAddressT> MatchmakerRedis::getPublishers() const
{
    return std::list<PublisherAddressT>();
}


std::list<std::string> MatchmakerRedis::getRouters() const
{
    return std::list<std::string>();
}


void MatchmakerRedis::addKey(const std::string& key)
{
    if (!m_redis.is_connected())
        throw std::runtime_error("Redis server is not connected!");

    std::vector<std::string> commands;
    {
        std::stringstream command;
        command << "SADD PUBLISHERS " << "\"" << key << "\"";
        commands.push_back(command.str());
        CLOG(DEBUG, "MatchmakerRedis") << "Executing: " << command.str();
    }
    {
        std::stringstream command;
        command << "EXPIRE PUBLISHERS " << m_conf.targetExpire();
        commands.push_back(command.str());
        CLOG(DEBUG, "MatchmakerRedis") << "Executing: " << command.str();
    }

    m_redis.send(commands, [](const cpp_redis::reply& reply){
        CLOG_IF(reply.is_error(), ERROR, "MatchmakerRedis") << "Commands execution failed!";
    });
}


void MatchmakerRedis::removeKey(const std::string& key)
{
    if (!m_redis.is_connected())
        throw std::runtime_error("Redis server is not connected!");

    std::stringstream command;
    command << "SREM PUBLISHERS " << key;
    m_redis.send(std::vector<std::string>{command.str()});

    CLOG(DEBUG, "MatchmakerRedis") << "Executing: " << command.str();
}
