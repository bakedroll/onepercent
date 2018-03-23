#include "osgGaming/LogManager.h"

#include <vector>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <assert.h>

namespace osgGaming
{
  LogManager::Ptr LogManager::m_instance;

  std::string currentTime(const char* format)
  {
    struct tm* now;
    time_t tim;
    time(&tim);
    now = localtime(&tim);

    char buffer[128];
    strftime(buffer, sizeof(buffer), format, now);

    return std::string(buffer);
  }

  void StdOutLogger::log(const std::string& text)
  {
    std::cout << text.c_str() << std::endl;
  }

  struct LogManager::Impl
  {
    Impl() : minSeverity(DEBUG) {}

    std::vector<osg::ref_ptr<Logger>> loggerList;
    Severety minSeverity;
  };

  LogManager::Ptr LogManager::getInstance()
  {
    if (!m_instance.valid())
      m_instance = new LogManager();

    return m_instance;
  }

  void LogManager::clearInstance()
  {
    m_instance->m->loggerList.clear();
    m_instance.release();
  }

  void LogManager::setMinSeverity(Severety severity)
  {
    m->minSeverity = severity;
  }

  void LogManager::log(Severety severety, const std::string& message)
  {
    if (severety < m->minSeverity)
      return;

    std::string now = currentTime("%Y-%m-%d %H:%M:%S");
    std::string sev;

    if (severety == DEBUG)
      sev = "DEBUG";
    else if (severety == INFO)
      sev = "INFO";
    else if (severety == WARNING)
      sev = "WARN";
    else if (severety == FATAL)
      sev = "FATAL";
    else
      assert(false);

    std::string text = "[" + now + " " + sev + "] " + message;

    for (std::vector<osg::ref_ptr<Logger>>::iterator it = m->loggerList.begin(); it != m->loggerList.end(); ++it)
      (*it)->log(text);
  }

  void LogManager::addLogger(osg::ref_ptr<Logger> logger)
  {
    m->loggerList.push_back(logger);
  }

  LogManager::LogManager()
    : osg::Referenced()
    , m(new Impl())
  {
  }

  LogManager::~LogManager()
  {
  }
}