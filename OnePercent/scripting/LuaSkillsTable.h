#pragma once

#include "scripting/LuaSkill.h"

#include <luaHelper/LuaTableMappedObject.h>
#include <luaHelper/ILuaStateManager.h>

namespace onep
{
  class LuaSkillsTable : public luaHelper::LuaTableMappedObject
  {
  public:
    using Ptr = std::shared_ptr<LuaSkillsTable>;

    explicit LuaSkillsTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaSkillsTable();

    const LuaSkill::Map& getSkills() const;
    LuaSkill::Ptr        getSkillByName(const std::string& name) const;

    void addSkill(luaHelper::ILuaStateManager::Ptr lua, const std::string& name, luabridge::LuaRef& ref);

  private:
    LuaSkill::Map m_skills;

  };
}