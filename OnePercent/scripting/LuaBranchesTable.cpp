#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaSkillsTable.h"

#include <QString>

namespace onep
{
  LuaBranchesTable::LuaBranchesTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaTableMappedObject(object, luaState)
  {
  }

  LuaBranchesTable::~LuaBranchesTable() = default;

const LuaSkillBranch::Map& LuaBranchesTable::getBranches() const
{
  return m_branches;
}

LuaSkillBranch::Ptr LuaBranchesTable::getBranchByName(const std::string& name) const
  {
    return getMappedElement<LuaSkillBranch>(name);
  }

  void LuaBranchesTable::addBranch(const std::string& name, luabridge::LuaRef& ref)
  {
    auto branch = addMappedElement<LuaSkillBranch>(name, ref);
    if (branch)
    {
      m_branches[name] = branch;
    }
  }

  LuaSkill::Ptr LuaBranchesTable::findSkill(const std::string& name) const
  {
    for (const auto& branch : m_branches)
    {
      auto skills = branch.second->getSkillsTable();
      if (skills->containsKey(name))
      {
        return skills->getSkillByName(name);
      }
    }

    OSGH_QLOG_WARN(QString("Skill '%1' not found").arg(QString::fromStdString(name)));
    return nullptr;
  }
}