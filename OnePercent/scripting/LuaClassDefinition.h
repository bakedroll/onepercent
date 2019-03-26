#pragma once

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#ifdef LUADOC_ENABLED
#include <luadoc/Namespace.h>
#else
#include <LuaBridge/LuaBridge.h>
#endif

namespace onep
{
  class LuaClassDefinition
  {
  public:
    virtual ~LuaClassDefinition() = default;
    virtual void registerClass(lua_State* state) = 0;

  protected:
#ifdef LUADOC_ENABLED
    static luadoc::Namespace getGlobalNamespace(lua_State* L);
#else
    static luabridge::Namespace getGlobalNamespace(lua_State* L);
#endif

  };
}
