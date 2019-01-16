#include "scripting/LuaCallbackRegistry.h"

#include <map>

namespace onep
{
  struct LuaCallbackRegistry::Impl
  {
    using IdCallbackMap = std::map<std::string, LuaCallback::Ptr>;
    using CallbackMap   = std::map<LuaDefines::Callback, IdCallbackMap>;

    CallbackMap callbacks;
  };

  LuaCallbackRegistry::LuaCallbackRegistry()
    : m(new Impl())
  {
  }

  LuaCallbackRegistry::~LuaCallbackRegistry() = default;

  void LuaCallbackRegistry::luaRegisterCallback(int id, luabridge::LuaRef param2, luabridge::LuaRef param3)
  {

  }

  void LuaCallbackRegistry::registerLuaCallbacks(LuaDefines::Callback id, LuaCallback::Ptr callback, std::string category)
  {
    m->callbacks[id][category] = callback;
  }
}