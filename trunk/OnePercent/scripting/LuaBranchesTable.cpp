#include "scripting/LuaBranchesTable.h"

namespace onep
{
  LuaBranchesTable::LuaBranchesTable(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
  {
    assert_return(object.isTable());

    std::map<std::string, LuaSkillBranch::Ptr> sortedMap;
    foreachElementDo([&](luabridge::LuaRef& key, luabridge::LuaRef& value)
    {
      assert_return(key.isString());
      LuaSkillBranch::Ptr branch = makeMappedElement<LuaSkillBranch>(key);
      sortedMap[branch->getBranchName()] = branch;
    });

    int id = 0;
    for (auto& it : sortedMap)
    {
      it.second->setBranchId(id++);
      m_branches.push_back(it.second);
    }
  }

  LuaBranchesTable::~LuaBranchesTable() = default;

  LuaSkillBranch::Ptr LuaBranchesTable::getBranchByIndex(int index)
  {
    return m_branches[index];
  }
}