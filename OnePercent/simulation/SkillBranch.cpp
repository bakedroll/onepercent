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

  BranchType SkillBranch::getType()
  {
    return m_type;
  }

  bool SkillBranch::callback(SimulationVisitor* visitor)
  {
    // traverse if branch activated
    if (visitor->getActivatedBranches()[int(m_type)])
      return true;

    return false;
  }
}