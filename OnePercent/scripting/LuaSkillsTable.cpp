#include "scripting/LuaSkillsTable.h"

namespace onep
{
  LuaSkillsTable::LuaSkillsTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaMapTable(object, luaState)
  {
    assert_return(object.isTable());
  }

  LuaSkillsTable::~LuaSkillsTable() = default;

  int LuaSkillsTable::getNumSkills() const
  {
    return int(m_skills.size());
  }

  LuaSkill::Ptr LuaSkillsTable::getSkillByIndex(int index) const
  {
    // TODO: optimize
    auto i = 0;
    for (auto& branch : m_skills)
    {
      if (i == index)
        return branch.second;
      ++i;
    }

    assert(false);
    return LuaSkill::Ptr();
  }

  LuaSkill::Ptr LuaSkillsTable::getSkillByName(const std::string& name) const
  {
    assert_return(m_skills.count(name) > 0, nullptr);
    return m_skills.find(name)->second;
  }

  void LuaSkillsTable::addSkill(LuaStateManager::Ptr lua, const std::string& name, luabridge::LuaRef& ref)
  {
    auto skill = std::make_shared<LuaSkill>(lua, ref);
    luaref()[name] = skill.get();
    m_skills[name] = skill;
  }

  LuaSkill::Map& LuaSkillsTable::getSkillsMap()
  {
    return m_skills;
  }
}
