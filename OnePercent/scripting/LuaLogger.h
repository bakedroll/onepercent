#pragma once

#include "scripting/LuaStateManager.h"

namespace onep
{
  class LuaLogger : public osg::Referenced, public LuaClassInstance
  {
  public:
    explicit LuaLogger(osgGaming::Injector& injector);
    ~LuaLogger();

    virtual void registerClass(lua_State* state) override;

    void lua_debug(std::string message);
    void lua_info(std::string message);
    void lua_warn(std::string message);
    void lua_fatal(std::string message);

  };
}