#include "SimulationVisitor.h"

namespace onep
{
  SimulationVisitor::SimulationVisitor(Type type)
    : osgUtil::UpdateVisitor()
    , m_type(type)
  {
    
  }

  bool* SimulationVisitor::getActivatedBranches()
  {
    return m_activatedBranches;
  }

  CountryValues::Ptr SimulationVisitor::getCountryValues()
  {
    return m_countryValues;
  }

  SimulationVisitor::Type SimulationVisitor::getType()
  {
    return m_type;
  }

  void SimulationVisitor::setActivatedBranches(bool* branches)
  {
    m_activatedBranches = branches;
  }

  void SimulationVisitor::setCountryValues(CountryValues::Ptr values)
  {
    m_countryValues = values;
  }
}