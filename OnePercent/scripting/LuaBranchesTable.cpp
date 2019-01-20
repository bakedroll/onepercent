#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaSkillsTable.h"

namespace onep
{
  LuaBranchesTable::LuaBranchesTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaMapTable(object, luaState)
  {
    assert_return(object.isTable());
  }

  LuaBranchesTable::~LuaBranchesTable() = default;

  LuaSkillBranch::Ptr LuaBranchesTable::getBranchByIndex(int index) const
  {
    // TODO: optimize
    auto i = 0;
    for (auto& branch : m_branches)
    {
      if (i == index)
        return branch.second;
      ++i;
    }

    assert(false);
    return LuaSkillBranch::Ptr();
  }

  LuaSkillBranch::Ptr LuaBranchesTable::getBranchByName(const std::string& name) const
  {
    return getMappedElement<LuaSkillBranch>(name);
  }

  int LuaBranchesTable::getNumBranches() const
  {
    return int(m_branches.size());
  }

  void LuaBranchesTable::addBranch(const std::string& name, luabridge::LuaRef& ref)
  {
    auto branch = addMappedElement<LuaSkillBranch>(name, ref);
    m_branches[name] = branch;
  }

  LuaSkill::Ptr LuaBranchesTable::findSkill(const std::string& name) const
  {
    for (const auto& branch : m_branches)
    {
      auto skills = branch.second->getSkillsTable();
      if (skills->contains(name))
      {
        return skills->getSkillByName(name);
      }
    }

    return nullptr;
  }
}