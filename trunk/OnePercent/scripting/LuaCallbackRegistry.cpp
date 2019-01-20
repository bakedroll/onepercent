#include "scripting/LuaCallbackRegistry.h"

namespace onep
{
  void LuaCallbackRegistry::Definition::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<LuaCallbackRegistry>("CallbackRegistry")
      .addFunction("on_event", &LuaCallbackRegistry::luaOnEvent)
      .endClass();
  }

  void LuaCallbackRegistry::luaOnEvent(int eventId, luabridge::LuaRef func)
  {
    const auto& it = m_callbacks.find(static_cast<LuaDefines::Callback>(eventId));
    assert_return(it != m_callbacks.end());

    it->second->addFunction(func);
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