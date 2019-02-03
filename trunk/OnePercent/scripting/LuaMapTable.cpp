#include "scripting/LuaMapTable.h"

namespace onep
{
  LuaMapTable::LuaMapTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
  {
  }

  LuaMapTable::~LuaMapTable() = default;
}