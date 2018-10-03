#include "scripting/LuaArrayTable.h"

namespace onep
{
  LuaArrayTable::LuaArrayTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
  {
  }

  LuaArrayTable::~LuaArrayTable()
  {
  }

  void LuaArrayTable::addEement(const luabridge::LuaRef& ref)
  {
    luaref().append(ref);
  }
}