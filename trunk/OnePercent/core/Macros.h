#pragma once

#include <osgGaming/LogManager.h>

#define OSGG_QLOG_DEBUG(msg) OSGG_LOG_DEBUG(std::string(msg.toLocal8Bit().constData()))
#define OSGG_QLOG_INFO(msg) OSGG_LOG_INFO(std::string(msg.toLocal8Bit().constData()))
#define OSGG_QLOG_WARN(msg) OSGG_LOG_WARN(std::string(msg.toLocal8Bit().constData()))
#define OSGG_QLOG_FATAL(msg) OSGG_LOG_FATAL(std::string(msg.toLocal8Bit().constData()))


#define ONEP_FOREACH(type, it, container) for (type::iterator it = container.begin(); it != container.end(); ++it)

#ifdef _DEBUG

#define assert_return(cond, ...) if (!(cond)) { assert(false); return __VA_ARGS__; }
#define assert_continue(cond, ...) if (!(cond)) { assert(false); continue; }

#else

#define assert_return(cond, ...)
#define assert_continue(cond, ...)

#endif