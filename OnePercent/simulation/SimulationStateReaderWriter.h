#pragma once

#include <string>

#include <osg/ref_ptr>

#include <osgHelper/Observable.h>

namespace onep
{
  class ModelContainer;
  class UpdateThread;

  class SimulationStateReaderWriter
  {
  public:
    SimulationStateReaderWriter();
    ~SimulationStateReaderWriter();

    bool saveState(
      const std::string& filename,
      const osg::ref_ptr<ModelContainer>& modelContainer,
      const osgHelper::Observable<int>::Ptr& oDay,
      const osgHelper::Observable<int>::Ptr& oNumSkillPoints);

    bool loadState(
      const std::string& filename,
      const osg::ref_ptr<ModelContainer>& modelContainer,
      const osgHelper::Observable<int>::Ptr& oDay,
      const osgHelper::Observable<int>::Ptr& oNumSkillPoints,
      UpdateThread* thread);

  };
}