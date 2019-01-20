#include "scripting/LuaCallback.h"

namespace onep
{
  void LuaCallback::addFunction(const luabridge::LuaRef& func)
  {
    m_funcs.push_back(func);
  }
}