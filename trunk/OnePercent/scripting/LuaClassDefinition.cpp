#include "scripting/LuaClassDefinition.h"

namespace onep
{
  luans::Namespace LuaClassDefinition::getGlobalNamespace(lua_State* L)
  {
    return luans::getGlobalNamespace(L);
  }
}
