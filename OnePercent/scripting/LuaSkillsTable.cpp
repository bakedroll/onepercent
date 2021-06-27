#include "scripting/LuaSkillsTable.h"

namespace onep
{
  LuaSkillsTable::LuaSkillsTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaTableMappedObject(object, luaState)
  {
  }

  LuaSkillsTable::~LuaSkillsTable() = default;

  const LuaSkill::Map& LuaSkillsTable::getSkills() const
  {
    return m_skills;
  }

  LuaSkill::Ptr LuaSkillsTable::getSkillByName(const std::string& name) const
  {
    assert_return(m_skills.count(name) > 0, nullptr);
    return m_skills.find(name)->second;
  }

  void LuaSkillsTable::addSkill(LuaStateManager::Ptr lua, const std::string& name, luabridge::LuaRef& ref)
  {
    auto skill = addUserDataElement<LuaSkill>(name, ref, lua);
    if (skill)
    {
      m_skills[name] = skill;
    }
  }
}
