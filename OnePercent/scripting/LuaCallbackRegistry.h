#pragma once

#include "scripting/LuaDefines.h"
#include "scripting/LuaCallback.h"

#include <string>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge/LuaBridge.h>

namespace onep
{
  class LuaCallbackRegistry
  {
  public:
    LuaCallbackRegistry();
    virtual ~LuaCallbackRegistry();

  public:
    void luaRegisterCallback(int id, luabridge::LuaRef param2, luabridge::LuaRef param3);

  protected:
    void registerLuaCallbacks(LuaDefines::Callback id, LuaCallback::Ptr callback, std::string category = "");
    void triggerLuaCallbacks();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}