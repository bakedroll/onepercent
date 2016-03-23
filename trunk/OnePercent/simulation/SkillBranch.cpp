#include "SkillBranch.h"
#include "SimulationVisitor.h"

namespace onep
{
  SkillBranch::SkillBranch(BranchType type)
    : Group()
    , SimulationCallback()
    , m_type(type)
  {
    setUpdateCallback(new Callback());
  }

  int SkillBranch::getNumSkills()
  {
    return int(m_skills.size());
  }

  Skill::Ptr SkillBranch::getSkill(int i)
  {
    return m_skills[i];
  }

  BranchType SkillBranch::getType()
  {
    return m_type;
  }

  void SkillBranch::addSkill(Skill::Ptr skill)
  {
    m_skills.push_back(skill);
    addChild(skill);
  }

  bool SkillBranch::callback(SimulationVisitor* visitor)
  {
    // traverse if branch activated
    if (visitor->getBranchActivated(m_type))
      return true;

    return false;
  }
}