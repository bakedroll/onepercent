#include "SkillBranch.h"

#include <assert.h>

namespace onep
{
  SkillBranch::SkillBranch(Type type)
    : m_type(type)
  {
    
  }

  void SkillBranch::addSkill(Skill::Ptr skill)
  {
    m_skills.push_back(skill);
  }

  SkillBranch::Type SkillBranch::getType()
  {
    return m_type;
  }

  void SkillBranch::forEachSkill(std::function<void(Skill::Ptr)> func)
  {
    for (auto& skill : m_skills)
      func(skill);
  }

  void SkillBranch::forEachActivatedSkill(std::function<void(Skill::Ptr)> func)
  {
    for (auto& skill : m_skills)
      if (skill->getActivated())
        func(skill);
  }

  std::string SkillBranch::getStringFromType(int type)
  {
    switch (type)
    {
    case BRANCH_CONTROL:
      return "control";
    case BRANCH_BANKS:
      return "banks";
    case BRANCH_CONCERNS:
      return "concerns";
    case BRANCH_MEDIA:
      return "media";
    case BRANCH_POLITICS:
      return "politics";
    case BRANCH_UNDEFINED:
    default:
      assert(false);
      return "undefined";
    }
  }

  SkillBranch::Type SkillBranch::getTypeFromString(std::string str)
  {
    if (str == "control")
      return BRANCH_CONTROL;
    if (str == "banks")
      return BRANCH_BANKS;
    if (str == "concerns")
      return BRANCH_CONCERNS;
    if (str == "media")
      return BRANCH_MEDIA;
    if (str == "politics")
      return BRANCH_POLITICS;

    assert(false);
    return BRANCH_UNDEFINED;
  }
}