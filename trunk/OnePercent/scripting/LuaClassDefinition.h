#pragma once

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace onep
{
  class LuaClassDefinition
  {
  public:
    virtual ~LuaClassDefinition() = default;
    virtual void registerClass(lua_State* state) = 0;
  };
}
