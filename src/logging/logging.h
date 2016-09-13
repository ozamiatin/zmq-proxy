#ifndef __LOGGING_H__
#define __LOGGING_H__

#ifdef USE_EASYLOGGING_

#include <easylogging++.h>

#define debug   DEBUG
#define info    INFO
#define warning WARNING
#define error   ERROR


#else // Use BOOST logging

#include <boost/log/trivial.hpp>
#define LOG BOOST_LOG_TRIVIAL
#endif // USE_EASYLOGGING_

#endif // __LOGGING_H__
