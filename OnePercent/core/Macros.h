#pragma once

#include <osgHelper/LogManager.h>

#define OSGG_QLOG_DEBUG(msg) OSGG_LOG_DEBUG(std::string(msg.toLocal8Bit().constData()))
#define OSGG_QLOG_INFO(msg) OSGG_LOG_INFO(std::string(msg.toLocal8Bit().constData()))
#define OSGG_QLOG_WARN(msg) OSGG_LOG_WARN(std::string(msg.toLocal8Bit().constData()))
#define OSGG_QLOG_FATAL(msg) OSGG_LOG_FATAL(std::string(msg.toLocal8Bit().constData()))
