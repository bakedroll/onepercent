#pragma once

#include "scripting/LuaObjectMapper.h"

#include <memory>
#include <vector>

namespace onep
{
  class LuaSkillsTable;
  class LuaSkill;

  class LuaSkillBranch : public LuaObjectMapper
  {
  public:
    typedef std::shared_ptr<LuaSkillBranch> Ptr;
    typedef std::vector<Ptr> List;

    explicit LuaSkillBranch(const luabridge::LuaRef& object);
    ~LuaSkillBranch();

    std::shared_ptr<LuaSkillsTable> getSkillsTable() const;
    int getBranchId() const;
    const std::string& getBranchName() const;
    int getCost() const;

    void setBranchId(int id);

  private:
    struct Impl;
    std::shared_ptr<Impl> m;

  };
}