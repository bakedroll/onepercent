#include "SkillBranch.h"
#include "SimulationVisitor.h"

namespace onep
{
  struct SkillBranch::Impl
  {
    Impl() : id(-1) {}

    int id;
    std::string name;
    int cost;

    Skill::List skills;
  };

  SkillBranch::SkillBranch(int id, const std::string& name, int costs)
    : Group()
    , SimulationCallback()
    , m(new Impl())
  {
    m->id = id;
    m->name = name;
    m->cost = costs;

    setUpdateCallback(new Callback());
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

  void SkillBranch::addSkill(Skill::Ptr skill)
  {
    m->skills.push_back(skill);
    addChild(skill);
  }

  bool SkillBranch::callback(SimulationVisitor* visitor)
  {
    // traverse if branch activated
    if (visitor->getBranchActivated(m->id))
      return true;

    return false;
  }
}