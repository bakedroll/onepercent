#pragma once

#include "scripting/LuaObjectMapper.h"
#include "scripting/LuaSkillBranch.h"

namespace onep
{
  class LuaBranchesTable : public LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaBranchesTable>;

    explicit LuaBranchesTable(const luabridge::LuaRef& object);
    ~LuaBranchesTable();

    LuaSkillBranch::Ptr getBranchByIndex(int index);
    int getNumBranches() const;

  private:
    LuaSkillBranch::List m_branches;

  };
}