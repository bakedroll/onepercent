#pragma once

#include <osgUtil/UpdateVisitor>
#include "CountryValues.h"
#include <osgGaming/Observable.h>

namespace onep
{
  typedef struct _skillBranchesActivated
  {
    osg::ref_ptr<osgGaming::Observable<bool>> oActivated[NUM_SKILLBRANCHES];
  } SkillBranchesActivated;

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

    bool getBranchActivated(BranchType type);
    CountryValues::Ptr getCountryValues();
    Type getType();

    void setActivatedBranches(SkillBranchesActivated* sba);
    void setCountryValues(CountryValues::Ptr values);

  private:
    Type m_type;

    SkillBranchesActivated* m_skillBranches;
    CountryValues::Ptr m_countryValues;
  };
}