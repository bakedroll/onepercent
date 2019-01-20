#pragma once

#include "scripting/LuaDefines.h"
#include "scripting/LuaCallback.h"
#include "scripting/LuaClassDefinition.h"

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
    class Definition : public LuaClassDefinition
    {
    public:
      void registerClass(lua_State* state) override;
    };

    void luaOnEvent(int eventId, luabridge::LuaRef func);

    template <typename... Args>
    void triggerLuaCallback(LuaDefines::Callback id, Args... args)
    {
      const auto& it = m_callbacks.find(id);
      assert_return(it != m_callbacks.end());

      it->second->trigger(args...);
    }

  protected:
    void registerLuaCallback(LuaDefines::Callback id, LuaCallback::Ptr callback = nullptr);

  private:
    using CallbackMap = std::map<LuaDefines::Callback, LuaCallback::Ptr>;

    CallbackMap m_callbacks;
  };
}