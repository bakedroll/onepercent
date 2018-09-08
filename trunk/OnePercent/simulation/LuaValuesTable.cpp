#include "simulation/LuaValuesTable.h"

namespace onep
{
  LuaValuesTable::LuaValuesTable(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
  {
    assert_return(object.isTable());
  }

  LuaValuesTable::~LuaValuesTable() = default;

  LuaValuesTable::ValuesMap LuaValuesTable::getValuesMap()
  {
    ValuesMap map;
    foreachElementDo([&](luabridge::LuaRef& key, luabridge::LuaRef& value)
    {
      assert_return(key.isString());
      assert_return(value.isNumber());
      map[key.tostring()] = float(value);
    });

    return map;
  }

  float LuaValuesTable::getValue(const std::string& name) const
  {
    return float(luaref()[name]);
  }

  void LuaValuesTable::setValue(const std::string& name, float value)
  {
    luaref()[name] = value;
  }
}