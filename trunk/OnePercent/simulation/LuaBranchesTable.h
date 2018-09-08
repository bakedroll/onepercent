#pragma once

#include "scripting/LuaObjectMapper.h"
#include "simulation/SkillBranch.h"

namespace onep
{
  class LuaBranchesTable : public LuaObjectMapper
  {
  public:
    explicit LuaBranchesTable(const luabridge::LuaRef& object);
    ~LuaBranchesTable();

    SkillBranch::Ptr getBranchByIndex(int index);

  private:
    SkillBranch::List m_branches;

  };
}