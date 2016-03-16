#pragma once

#include "core/Globals.h"
#include "SimulationCallback.h"

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

    SkillBranch(BranchType type);

    BranchType getType();

    virtual bool callback(SimulationVisitor* visitor) override;

  private:
    BranchType m_type;

  };
}