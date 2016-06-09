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
    addHost("PUBLISHERS", host.first + "," + host.second);
}


void MatchmakerRedis::unregisterPublisher(const PublisherAddressT& host)
{
    removeHost("PUBLISHERS", host.first + "," + host.second);
}


void MatchmakerRedis::registerRouter(const std::string& hostname)
{
    addHost("ROUTERS", hostname);
}


void MatchmakerRedis::unregisterRouter(const std::string& hostname)
{
    removeHost("ROUTERS", hostname);
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

    CLOG(DEBUG, "MatchmakerRedis") << "Executing: " << "SADD " << key << " " << host;
    m_redis.send({"SADD", key, host}, [](const cpp_redis::reply& reply){
        CLOG_IF(reply.is_error(), ERROR, "MatchmakerRedis") << "Command execution failed!";
    });

    CLOG(DEBUG, "MatchmakerRedis") << "Executing: " << "EXPIRE " << key;
    m_redis.send({"EXPIRE", key, std::to_string(m_conf.targetExpire())},
                 [](const cpp_redis::reply& reply){
        CLOG_IF(reply.is_error(), ERROR, "MatchmakerRedis") << "Command execution failed!";
    });
}


void MatchmakerRedis::removeHost(const std::string& key, const std::string& host)
{
    if (!m_redis.is_connected())
        throw std::runtime_error("Redis server is not connected!");

    CLOG(DEBUG, "MatchmakerRedis") << "Executing: " << "SREM " << key << " " << host;
    m_redis.send({"SREM", key, host}, [](const cpp_redis::reply& reply){
        CLOG_IF(reply.is_error(), ERROR, "MatchmakerRedis") << "Command execution failed!";
    });
}
