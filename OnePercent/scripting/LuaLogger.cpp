#include "LuaLogger.h"

#include <osgGaming/LogManager.h>

namespace onep
{
  LuaLogger::LuaLogger(osgGaming::Injector& injector)
    : osg::Referenced()
    , LuaClassInstance("log")
  {
  }

  LuaLogger::~LuaLogger()
  {
  }

  void LuaLogger::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<LuaLogger>("LuaLogger")
      .addFunction("debug", &LuaLogger::lua_debug)
      .addFunction("info", &LuaLogger::lua_info)
      .addFunction("warn", &LuaLogger::lua_warn)
      .addFunction("fatal", &LuaLogger::lua_fatal)
      .endClass();
  }

  void LuaLogger::lua_debug(std::string message)
  {
    OSGG_LOG_DEBUG(message);
  }

  void LuaLogger::lua_info(std::string message)
  {
    OSGG_LOG_INFO(message);
  }

  void LuaLogger::lua_warn(std::string message)
  {
    OSGG_LOG_WARN(message);
  }

  void LuaLogger::lua_fatal(std::string message)
  {
    OSGG_LOG_FATAL(message);
  }
}