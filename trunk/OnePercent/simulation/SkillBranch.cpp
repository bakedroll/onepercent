#include "SkillBranch.h"

#include "core/Macros.h"

namespace onep
{
  struct SkillBranch::Impl
  {
    Impl() {}

    int id;
    std::string name;
    int cost;

    Skill::List skills;
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

    for (luabridge::Iterator it(skillsRef); !it.isNil(); ++it)
    {
      luabridge::LuaRef skillRef = *it;
      assert_continue(skillRef.isTable());

      m->skills.push_back(new Skill(skillRef));
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

  Skill::Ptr SkillBranch::getSkill(int i) const
  {
    return m->skills[i];
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