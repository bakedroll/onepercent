#include "simulation/LuaSkillsTable.h"

namespace onep
{
  LuaSkillsTable::LuaSkillsTable(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
  {
    assert_return(object.isTable());

    std::map<std::string, Skill::Ptr> sortedMap;

    foreachElementDo([&](luabridge::LuaRef& key, luabridge::LuaRef& value)
    {
      assert_return(key.isString());
      Skill::Ptr skill = makeMappedElement<Skill>(key);
      sortedMap[skill->getSkillName()] = skill;
    });

    for (auto& it : sortedMap)
    {
      m_skills.push_back(it.second);
    }
  }

  LuaSkillsTable::~LuaSkillsTable() = default;

  int LuaSkillsTable::getNumSkills() const
  {
    return int(m_skills.size());
  }

  Skill::Ptr LuaSkillsTable::getSkillByIndex(int i) const
  {
    return m_skills[i];
  }

  Skill::Ptr LuaSkillsTable::getSkillByName(std::string name) const
  {
    return getMappedElement<Skill>(name);
  }
}
