#include "LuaStateManager.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge.h>

namespace onep
{
  struct LuaStateManager::Impl
  {
    Impl()
    {}

    lua_State* state;
  };

  LuaStateManager::LuaStateManager(osgGaming::Injector& injector)
    : m(new Impl())
  {
    m->state = luaL_newstate();
  }

  LuaStateManager::~LuaStateManager()
  {
    lua_close(m->state);
  }
}