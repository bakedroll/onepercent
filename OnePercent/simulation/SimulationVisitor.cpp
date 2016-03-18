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
    return &m_activatedBranches[0];
  }

  CountryValues::Ptr SimulationVisitor::getCountryValues()
  {
    return m_countryValues;
  }

  SimulationVisitor::Type SimulationVisitor::getType()
  {
    return m_type;
  }

  void SimulationVisitor::setActivatedBranches(osgGaming::Observable<bool>* branches)
  {
    for (int i = 0; i < NUM_SKILLBRANCHES; i++)
      m_activatedBranches[i] = branches[i].get();
  }

  void SimulationVisitor::setCountryValues(CountryValues::Ptr values)
  {
    m_countryValues = values;
  }
}