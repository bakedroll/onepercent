#include "scripting/LuaClassDefinition.h"

namespace onep
{
#ifdef LUADOC_ENABLED
  luadoc::Namespace LuaClassDefinition::getGlobalNamespace(lua_State* L)
  {
    return luadoc::Namespace::getGlobalNamespace(L);
  }
#else
  luabridge::Namespace LuaClassDefinition::getGlobalNamespace(lua_State* L)
  {
    return luabridge::getGlobalNamespace(L);
  }
#endif
}
