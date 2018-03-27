#pragma once

#include <osgGaming/LogManager.h>

#define OSGG_QLOG_DEBUG(msg) OSGG_LOG_DEBUG(msg.toStdString())
#define OSGG_QLOG_INFO(msg) OSGG_LOG_INFO(msg.toStdString())
#define OSGG_QLOG_WARN(msg) OSGG_LOG_WARN(msg.toStdString())
#define OSGG_QLOG_FATAL(msg) OSGG_LOG_FATAL(msg.toStdString())