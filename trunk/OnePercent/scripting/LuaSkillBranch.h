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
    typedef std::map<std::string, Ptr> Map;

    explicit LuaSkillBranch(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaSkillBranch();

    std::shared_ptr<LuaSkillsTable> getSkillsTable() const;
    const std::string& getBranchName() const;
    int getCost() const;

  private:
    struct Impl;
    std::shared_ptr<Impl> m;

  };
}