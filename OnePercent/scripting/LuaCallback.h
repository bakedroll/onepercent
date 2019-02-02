#pragma once

#include "scripting/LuaStateManager.h"

#include <memory>
#include <vector>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge/LuaBridge.h>

namespace onep
{
  class LuaCallback
  {
  public:
    using Ptr = std::shared_ptr<LuaCallback>;

    void addFunction(const luabridge::LuaRef& func);

    template<typename... Args>
    void trigger(const LuaStateManager::Ptr& lua, Args... args) const
    {
      lua->safeExecute([this, &args...]()
      {
        for (const auto& func : m_funcs)
        {
          func(args...);
        }
      });
    }

  private:
    using LuaFuncList = std::vector<luabridge::LuaRef>;

    LuaFuncList m_funcs;
  };

}