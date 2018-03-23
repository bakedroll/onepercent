#pragma once

#include "SimulationCallback.h"
#include "Skill.h"

#include <osg/Group>
#include <osg/ref_ptr>

#include <map>

namespace onep
{
  class SkillBranch : public osg::Group, public SimulationCallback
  {
  public:
    typedef osg::ref_ptr<SkillBranch> Ptr;
    typedef std::map<int, Ptr> Map;

    SkillBranch(int id, const std::string& name, int costs);

    int getBranchId() const;
    std::string getBranchName() const;
    int getNumSkills() const;
    Skill::Ptr getSkill(int i) const;

    int getCosts() const;

    void addSkill(Skill::Ptr skill);

    virtual bool callback(SimulationVisitor* visitor) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}