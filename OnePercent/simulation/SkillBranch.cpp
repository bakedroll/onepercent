#include "SkillBranch.h"

#include <osgGaming/Macros.h>

namespace onep
{
  struct SkillBranch::Impl
  {
    Impl() {}

    int id;
    std::string name;
    int cost;

    Skill::List skills;
    std::map<std::string, int> nameIndexMap;
  };

  SkillBranch::SkillBranch(const luabridge::LuaRef& object, int id)
    : osg::Referenced()
    , LuaObjectMapper(object)
    , m(new Impl())
  {
    luabridge::LuaRef nameRef = object["name"];
    luabridge::LuaRef costRef = object["cost"];
    luabridge::LuaRef skillsRef = object["skills"];

    assert_return(nameRef.isString());
    assert_return(costRef.isNumber());
    assert_return(skillsRef.isTable());

    m->id = id;
    m->name = nameRef.tostring();
    m->cost = costRef;

    std::map<std::string, Skill::Ptr> sortedMap;
    for (luabridge::Iterator it(skillsRef); !it.isNil(); ++it)
    {
      luabridge::LuaRef skillRef = *it;
      assert_continue(skillRef.isTable());

      Skill::Ptr skill = new Skill(skillRef);
      sortedMap[skill->getSkillName()] = skill;
    }

    for (auto& it : sortedMap)
    {
      m->skills.push_back(it.second);
      m->nameIndexMap[it.first] = int(m->skills.size()) - 1;
    }
  }

  SkillBranch::~SkillBranch()
  {
  }

  int SkillBranch::getBranchId() const
  {
    return m->id;
  }

  const std::string& SkillBranch::getBranchName() const
  {
    return m->name;
  }

  int SkillBranch::getNumSkills() const
  {
    return int(m->skills.size());
  }

  Skill::Ptr SkillBranch::getSkillByIndex(int i) const
  {
    return m->skills[i];
  }

  Skill::Ptr SkillBranch::getSkillByName(std::string name) const
  {
    assert_return(m->nameIndexMap.count(name) > 0, nullptr);
    return m->skills[m->nameIndexMap[name]];
  }

  int SkillBranch::getCost() const
  {
    return m->cost;
  }

  void SkillBranch::writeObject(luabridge::LuaRef& object) const
  {
  }

  void SkillBranch::readObject(const luabridge::LuaRef& object)
  {
  }
}