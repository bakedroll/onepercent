#include "scripting/LuaBranchesTable.h"
#include "core/Enums.h"

namespace onep
{
  LuaBranchesTable::LuaBranchesTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
  {
    addVisitorFunc(static_cast<int>(ModelTraversalType::INITIALIZE_DATA), [this](luabridge::LuaRef&)
    {
      assert_return(luaref().isTable());

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
    });
  }

  LuaBranchesTable::~LuaBranchesTable() = default;

  LuaSkillBranch::Ptr LuaBranchesTable::getBranchByIndex(int index) const
  {
    return m_branches[index];
  }

  LuaSkillBranch::Ptr LuaBranchesTable::getBranchByName(const std::string& name) const
  {
    return getMappedElement<LuaSkillBranch>(name);
  }

  int LuaBranchesTable::getNumBranches() const
  {
    return int(m_branches.size());
  }
}