#pragma once

#include "scripting/LuaDefines.h"
#include "scripting/LuaCallback.h"
#include "scripting/LuaBridgeDefinition.h"
#include "scripting/LuaStateManager.h"

#include <osgGaming/Macros.h>
#include <osg/Referenced>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge/LuaBridge.h>

namespace onep
{
  class LuaCallbackRegistry : public osg::Referenced
  {
  public:
    class Definition : public LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    explicit LuaCallbackRegistry(const LuaStateManager::Ptr& lua);

    void luaOnEvent(int eventId, luabridge::LuaRef func);

    template <typename... Args>
    void triggerLuaCallback(LuaDefines::Callback id, Args... args)
    {
      const auto& it = m_callbacks.find(id);
      assert_return(it != m_callbacks.end());

      it->second->trigger(m_lua, args...);
    }

  protected:
    void registerLuaCallback(LuaDefines::Callback id, LuaCallback::Ptr callback = nullptr);

  private:
    using CallbackMap = std::map<LuaDefines::Callback, LuaCallback::Ptr>;

    CallbackMap           m_callbacks;
    LuaStateManager::Ptr  m_lua;
  };
}