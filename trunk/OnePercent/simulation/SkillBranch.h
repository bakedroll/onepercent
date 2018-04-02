#pragma once

#include "Skill.h"

#include <osg/ref_ptr>

#include <memory>

namespace onep
{
  class SkillBranch : public osg::Referenced
  {
  public:
    typedef osg::ref_ptr<SkillBranch> Ptr;
    typedef std::vector<Ptr> List;

    SkillBranch(int id, const std::string& name, int costs);

    int getBranchId() const;
    const std::string& getBranchName() const;
    int getNumSkills() const;
    Skill::Ptr getSkill(int i) const;

    int getCost() const;

    void addSkill(Skill::Ptr skill);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}