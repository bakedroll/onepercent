#include "scripting/LuaBranchValuesTable.h"
#include "core/Enums.h"

namespace onep
{
  LuaBranchValuesTable::LuaBranchValuesTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
  {
    assert_return(object.isTable());
    makeAllMappedElements<LuaValuesTable>();
  }

  LuaBranchValuesTable::~LuaBranchValuesTable() = default;

  LuaBranchValuesTable::BranchValuesMap LuaBranchValuesTable::getBranchValuesMap()
  {
    BranchValuesMap map;
    foreachElementDo([&](luabridge::LuaRef& key, luabridge::LuaRef& value)
    {
      assert_return(key.isString());
      assert_return(value.isTable());
      map[key.tostring()] = getMappedElement<LuaValuesTable>(key)->getValuesMap();
    });

    return map;
  }

  LuaValuesTable::Ptr LuaBranchValuesTable::getBranch(const std::string& name) const
  {
    return getMappedElement<LuaValuesTable>(name);
  }
}
