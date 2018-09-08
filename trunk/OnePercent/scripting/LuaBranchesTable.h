#pragma once

#include "scripting/LuaObjectMapper.h"
#include "scripting/LuaSkillBranch.h"

namespace onep
{
  class LuaBranchesTable : public LuaObjectMapper
  {
  public:
    explicit LuaBranchesTable(const luabridge::LuaRef& object);
    ~LuaBranchesTable();

    LuaSkillBranch::Ptr getBranchByIndex(int index);

  private:
    LuaSkillBranch::List m_branches;

  };
}