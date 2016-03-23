#pragma once

#include "core/Globals.h"
#include "SimulationCallback.h"

#include <osg/Group>
#include <osg/ref_ptr>

#include <map>
#include "Skill.h"


namespace onep
{
  class SkillBranch : public osg::Group, public SimulationCallback
  {
  public:
    typedef osg::ref_ptr<SkillBranch> Ptr;
    typedef std::map<int, Ptr> Map;

    SkillBranch(BranchType type);

    int getNumSkills();
    Skill::Ptr getSkill(int i);
    BranchType getType();

    void addSkill(Skill::Ptr skill);

    virtual bool callback(SimulationVisitor* visitor) override;

  private:
    BranchType m_type;

    Skill::List m_skills;

  };
}