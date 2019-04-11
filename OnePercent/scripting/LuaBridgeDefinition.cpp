#include "scripting/LuaBridgeDefinition.h"

namespace onep
{
  luans::Namespace LuaBridgeDefinition::getGlobalNamespace(lua_State* L)
  {
    return luans::getGlobalNamespace(L);
  }
}
