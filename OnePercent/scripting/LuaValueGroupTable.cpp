#include "scripting/LuaValueGroupTable.h"

namespace onep
{
  LuaValueGroupTable::LuaValueGroupTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaTableMappedObject(object, luaState)
  {
    assert_return(object.isTable());
  }

  LuaValueGroupTable::~LuaValueGroupTable() = default;

  LuaValueGroupTable::ValuesMap LuaValueGroupTable::getMap()
  {
    ValuesMap map;
    foreachElementDo([&](luabridge::LuaRef& key, luabridge::LuaRef& value)
    {
      assert_return(key.isString());

      if (!value.isNumber())
      {
        return;
      }
      map[key.tostring()] = float(value);
    });

    return map;
  }

  float LuaValueGroupTable::getValue(const std::string& name) const
  {
    return float(luaRef()[name]);
  }

  void LuaValueGroupTable::setValue(const std::string& name, float value)
  {
    luaRef()[name] = value;
  }

  std::shared_ptr<LuaValueGroupTable> LuaValueGroupTable::getGroup(const std::string& name)
  {
    if (m_groups.count(name) > 0)
    {
      return m_groups[name];
    }

    const auto group = newMappedElement<LuaValueGroupTable>(name);
    m_groups[name] = group;
    return group;
  }

  const LuaValueGroupTable::GroupsMap& LuaValueGroupTable::getAllGroups() const
  {
    return m_groups;
  }
}
