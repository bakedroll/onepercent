#include "scripting/LuaMapTable.h"

namespace onep
{
  LuaMapTable::LuaMapTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
  {
  }

  LuaMapTable::~LuaMapTable() = default;

  bool LuaMapTable::contains(const std::string& key) const
  {
    return !luaref()[key].isNil();
  }

  void LuaMapTable::insert(const std::string& key, const luabridge::LuaRef& elem)
  {
    luaref()[key] = elem;
  }

  luabridge::LuaRef LuaMapTable::getElement(const std::string& key) const
  {
    return luaref()[key];
  }
}