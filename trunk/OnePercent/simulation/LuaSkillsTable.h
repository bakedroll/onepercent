#pragma once

#include "scripting/LuaObjectMapper.h"
#include "simulation/Skill.h"

namespace onep
{
  class LuaSkillsTable : public LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaSkillsTable>;

    explicit LuaSkillsTable(const luabridge::LuaRef& object);
    ~LuaSkillsTable();

    int getNumSkills() const;
    Skill::Ptr getSkillByIndex(int i) const;
    Skill::Ptr getSkillByName(std::string name) const;

  private:
    Skill::List m_skills;

  };
}