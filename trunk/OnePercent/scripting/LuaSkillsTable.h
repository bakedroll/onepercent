#pragma once

#include "scripting/LuaObjectMapper.h"
#include "scripting/LuaSkill.h"

namespace onep
{
  class LuaSkillsTable : public LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaSkillsTable>;

    explicit LuaSkillsTable(const luabridge::LuaRef& object);
    ~LuaSkillsTable();

    int getNumSkills() const;
    LuaSkill::Ptr getSkillByIndex(int i) const;
    LuaSkill::Ptr getSkillByName(std::string name) const;

  private:
    LuaSkill::List m_skills;

  };
}