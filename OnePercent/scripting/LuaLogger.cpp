#include "LuaLogger.h"

#include <osgHelper/LogManager.h>

namespace onep
{
  void LuaLogger::Definition::registerDefinition(lua_State* state)
  {
    getGlobalNamespace(state)
      .beginClass<LuaLogger>("LuaLogger")
      .addFunction("debug", &LuaLogger::lua_debug)
      .addFunction("info", &LuaLogger::lua_info)
      .addFunction("warn", &LuaLogger::lua_warn)
      .addFunction("fatal", &LuaLogger::lua_fatal)
      .endClass();
  }

  LuaLogger::LuaLogger(osgHelper::ioc::Injector& injector)
    : osg::Referenced()
  {
  }

  LuaLogger::~LuaLogger() = default;

  void LuaLogger::lua_debug(std::string message)
  {
    OSGH_LOG_DEBUG(message);
  }

  void LuaLogger::lua_info(std::string message)
  {
    OSGH_LOG_INFO(message);
  }

  void LuaLogger::lua_warn(std::string message)
  {
    OSGH_LOG_WARN(message);
  }

  void LuaLogger::lua_fatal(std::string message)
  {
    OSGH_LOG_FATAL(message);
  }
}