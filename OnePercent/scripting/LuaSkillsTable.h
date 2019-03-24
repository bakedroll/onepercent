#pragma once

#include "scripting/LuaMapTable.h"
#include "scripting/LuaSkill.h"
#include "scripting/LuaStateManager.h"

namespace onep
{
  class LuaSkillsTable : public LuaMapTable
  {
  public:
    using Ptr = std::shared_ptr<LuaSkillsTable>;

    explicit LuaSkillsTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaSkillsTable();

    const LuaSkill::Map& getSkills() const;
    LuaSkill::Ptr        getSkillByName(const std::string& name) const;

    void addSkill(LuaStateManager::Ptr lua, const std::string& name, luabridge::LuaRef& ref);

    LuaSkill::Map& getSkillsMap();

  private:
    LuaSkill::Map m_skills;

  };
}