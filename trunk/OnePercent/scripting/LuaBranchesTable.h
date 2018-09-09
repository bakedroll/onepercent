#pragma once

#include "scripting/LuaObjectMapper.h"
#include "scripting/LuaSkillBranch.h"

namespace onep
{
  class LuaBranchesTable : public LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaBranchesTable>;

    explicit LuaBranchesTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaBranchesTable();

    LuaSkillBranch::Ptr getBranchByIndex(int index) const;
    LuaSkillBranch::Ptr getBranchByName(const std::string& name) const;
    int getNumBranches() const;

  protected:
    virtual void onTraverse(int type, luabridge::LuaRef& object) override;

  private:
    LuaSkillBranch::List m_branches;

  };
}