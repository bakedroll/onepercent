#pragma once

#include "scripting/LuaObjectMapper.h"

#include <memory>
#include <vector>

namespace onep
{
  class LuaSkillsTable;
  class Skill;

  class SkillBranch : public LuaObjectMapper
  {
  public:
    typedef std::shared_ptr<SkillBranch> Ptr;
    typedef std::vector<Ptr> List;

    explicit SkillBranch(const luabridge::LuaRef& object);
    ~SkillBranch();

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