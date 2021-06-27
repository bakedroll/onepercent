#pragma once

#include "scripting/LuaTableMappedObject.h"
#include "scripting/LuaSkillBranch.h"
#include "scripting/LuaSkill.h"

namespace onep
{
  class LuaBranchesTable : public LuaTableMappedObject
  {
  public:
    using Ptr = std::shared_ptr<LuaBranchesTable>;

    explicit LuaBranchesTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaBranchesTable();

    const LuaSkillBranch::Map& getBranches() const;
    LuaSkillBranch::Ptr        getBranchByName(const std::string& name) const;
    void                       addBranch(const std::string& name, luabridge::LuaRef& ref);

    LuaSkill::Ptr findSkill(const std::string& name) const;

  private:
    LuaSkillBranch::Map m_branches;

  };
}