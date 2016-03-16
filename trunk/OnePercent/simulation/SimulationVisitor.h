#pragma once

#include <osgUtil/UpdateVisitor>
#include "CountryValues.h"

namespace onep
{
  class SimulationVisitor : public osgUtil::UpdateVisitor
  {
  public:
    typedef osg::ref_ptr<SimulationVisitor> Ptr;

    typedef enum _type
    {
      APPLY_SKILLS,
      AFFECT_NEIGHBORS,
      PROGRESS_COUNTRIES
    } Type;

    SimulationVisitor(Type type);

    bool* getActivatedBranches();
    CountryValues::Ptr getCountryValues();
    Type getType();

    void setActivatedBranches(bool* branches);
    void setCountryValues(CountryValues::Ptr values);

  private:
    Type m_type;

    bool* m_activatedBranches;
    CountryValues::Ptr m_countryValues;
  };
}