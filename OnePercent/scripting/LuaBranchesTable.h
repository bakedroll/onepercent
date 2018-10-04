#pragma once

#include "scripting/LuaMapTable.h"
#include "scripting/LuaSkillBranch.h"

namespace onep
{
  class LuaBranchesTable : public LuaMapTable
  {
  public:
    using Ptr = std::shared_ptr<LuaBranchesTable>;

    explicit LuaBranchesTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaBranchesTable();

    LuaSkillBranch::Ptr getBranchByIndex(int index) const;
    LuaSkillBranch::Ptr getBranchByName(const std::string& name) const;
    int getNumBranches() const;

    void addBranch(const std::string& name, luabridge::LuaRef& ref);

  private:
    LuaSkillBranch::Map m_branches;

  };
}