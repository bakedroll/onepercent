#pragma once

#include "Skill.h"

#include <osg/Referenced>
#include <osg/ref_ptr>

#include <map>
#include <functional>

namespace onep
{
  class SkillBranch : public osg::Referenced
  {
  public:
    typedef enum _type
    {
      BRANCH_BANKS = 0,
      BRANCH_CONTROL = 1,
      BRANCH_MEDIA = 2,
      BRANCH_CONCERNS = 3,
      BRANCH_POLITICS = 4,
      BRANCH_UNDEFINED
    } Type;

    typedef osg::ref_ptr<SkillBranch> Ptr;
    typedef std::map<int, Ptr> Map;

    SkillBranch(Type type);

    void addSkill(Skill::Ptr skill);

    Type getType();

    void forEachSkill(std::function<void(Skill::Ptr)> func);
    void forEachActivatedSkill(std::function<void(Skill::Ptr)> func);

    static std::string getStringFromType(int type);
    static Type getTypeFromString(std::string str);

  private:
    Type m_type;
    Skill::List m_skills;
  };
}