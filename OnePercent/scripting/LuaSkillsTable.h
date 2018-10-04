#pragma once

#include "scripting/LuaMapTable.h"
#include "scripting/LuaSkill.h"

namespace onep
{
  class LuaSkillsTable : public LuaMapTable
  {
  public:
    using Ptr = std::shared_ptr<LuaSkillsTable>;

    explicit LuaSkillsTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaSkillsTable();

    int getNumSkills() const;
    LuaSkill::Ptr getSkillByIndex(int index) const;
    LuaSkill::Ptr getSkillByName(std::string name) const;

    void addSkill(const std::string& name, luabridge::LuaRef& ref);

  private:
    LuaSkill::Map m_skills;

  };
}