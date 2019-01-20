#pragma once

#include "scripting/LuaStateManager.h"

namespace onep
{
  class LuaLogger : public osg::Referenced
  {
  public:
    class Definition : public LuaClassDefinition
    {
    public:
      void registerClass(lua_State* state) override;
    };

    explicit LuaLogger(osgGaming::Injector& injector);
    ~LuaLogger();

    void lua_debug(std::string message);
    void lua_info(std::string message);
    void lua_warn(std::string message);
    void lua_fatal(std::string message);

  };
}