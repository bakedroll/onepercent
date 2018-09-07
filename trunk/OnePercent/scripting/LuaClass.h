#pragma once

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace onep
{
  class LuaClass
  {
  public:
    virtual ~LuaClass() = default;
    virtual void registerClass(lua_State* state) = 0;
  };
}
