#include "scripting/LuaValuesDefTable.h"
#include "scripting/LuaValueDef.h"

namespace onep
{
  LuaValuesDefTable::LuaValuesDefTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
  {
    assert_return(object.isTable());
  }

  LuaValuesDefTable::~LuaValuesDefTable() = default;

  void LuaValuesDefTable::addValue(const std::string& name, luabridge::LuaRef ref)
  {
    addMappedElement<LuaValueDef>(name, ref);
  }
}