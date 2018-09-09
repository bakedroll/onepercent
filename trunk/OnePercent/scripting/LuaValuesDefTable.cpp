#include "scripting/LuaValuesDefTable.h"

namespace onep
{
  LuaValuesDefTable::LuaValuesDefTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
  {
    assert_return(object.isTable());
  }

  LuaValuesDefTable::~LuaValuesDefTable() = default;
}