#include "simulation/LuaBranchesTable.h"

namespace onep
{
  LuaBranchesTable::LuaBranchesTable(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
  {
    assert_return(object.isTable());

    std::map<std::string, SkillBranch::Ptr> sortedMap;
    foreachElementDo([&](luabridge::LuaRef& key, luabridge::LuaRef& value)
    {
      assert_return(key.isString());
      SkillBranch::Ptr branch = makeMappedElement<SkillBranch>(key);
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

  SkillBranch::Ptr LuaBranchesTable::getBranchByIndex(int index)
  {
    return m_branches[index];
  }
}