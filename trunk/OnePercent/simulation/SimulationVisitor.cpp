#include "SimulationVisitor.h"

namespace onep
{
  SimulationVisitor::SimulationVisitor(Type type)
    : osgUtil::UpdateVisitor()
    , m_type(type)
  {
    
  }

  bool SimulationVisitor::getBranchActivated(BranchType type)
  {
    return m_skillBranches->oActivated[type]->get();
  }

  CountryValues::Ptr SimulationVisitor::getCountryValues()
  {
    return m_countryValues;
  }

  SimulationVisitor::Type SimulationVisitor::getType()
  {
    return m_type;
  }

  void SimulationVisitor::setActivatedBranches(SkillBranchesActivated* sba)
  {
    m_skillBranches = sba;
  }

  void SimulationVisitor::setCountryValues(CountryValues::Ptr values)
  {
    m_countryValues = values;
  }
}