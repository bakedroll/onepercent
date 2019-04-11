#include "scripting/LuaCallbackRegistry.h"

namespace onep
{
  void LuaCallbackRegistry::Definition::registerDefinition(lua_State* state)
  {
    getGlobalNamespace(state)
      .beginClass<LuaCallbackRegistry>("CallbackRegistry")
      .addFunction("on_event", &LuaCallbackRegistry::luaOnEvent)
      .endClass();
  }

  LuaCallbackRegistry::LuaCallbackRegistry(const LuaStateManager::Ptr& lua)
    : m_lua(lua)
  {
  }

  void LuaCallbackRegistry::luaOnEvent(int eventId, luabridge::LuaRef func)
  {
    const auto& it = m_callbacks.find(static_cast<LuaDefines::Callback>(eventId));
    assert_return(it != m_callbacks.end());

    m_lua->safeExecute([this, &it, &func]()
    {
      assert_return(m_lua->checkIsType(func, LUA_TFUNCTION));
      it->second->addFunction(func);
    });
  }

  void LuaCallbackRegistry::registerLuaCallback(LuaDefines::Callback id, LuaCallback::Ptr callback)
  {
    if (!callback)
    {
      callback = std::make_shared<LuaCallback>();
    }

    m_callbacks[id] = callback;
  }
}