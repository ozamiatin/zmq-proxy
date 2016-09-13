#ifndef __LOGGING_INIT_H__
#define __LOGGING_INIT_H__

#ifdef USE_EASYLOGGING_

#include <easylogging++.h>

INITIALIZE_EASYLOGGINGPP

#include "logging.h"


void init_logging(int argc, char** argv)
{
    START_EASYLOGGINGPP(argc, argv);
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format,
                                       "%datetime{%H:%m:%s} %func %level    %msg");
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
}


#else // Use BOOST logging

#include "logging.h"


void init_logging(int /*argc*/, char** /*argv*/)
{
    boost::log::core::get()->set_filter
    (
#ifdef NDEBUG
        boost::log::trivial::severity >= boost::log::trivial::info
#else
        boost::log::trivial::severity >= boost::log::trivial::debug
#endif
    );
}

#endif // USE_EASYLOGGING_
#endif // __LOGGING_INIT_H__
