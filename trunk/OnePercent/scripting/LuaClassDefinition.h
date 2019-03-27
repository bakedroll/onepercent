#pragma once

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#ifdef LUADOC_ENABLED
#include <luadoc/Namespace.h>
#define luans luadoc
#else
#include <LuaBridge/LuaBridge.h>
#define luans luabridge
#endif

namespace onep
{
  class LuaClassDefinition
  {
  public:
    virtual ~LuaClassDefinition() = default;
    virtual void registerClass(lua_State* state) = 0;

  protected:
    static luans::Namespace getGlobalNamespace(lua_State* L);

  };
}
