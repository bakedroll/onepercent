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

  LuaSkill::Ptr LuaSkillsTable::getSkillByName(std::string name) const
  {
    return getMappedElement<LuaSkill>(name);
  }

  void LuaSkillsTable::addSkill(const std::string& name, luabridge::LuaRef& ref)
  {
    auto skill = addMappedElement<LuaSkill>(name, ref);
    m_skills[name] = skill;
  }
}
