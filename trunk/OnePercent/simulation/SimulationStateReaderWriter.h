#pragma once

#include <string>

#include <osg/ref_ptr>

#include <osgGaming/Observable.h>

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
      const osgGaming::Observable<int>::Ptr& oDay,
      const osgGaming::Observable<int>::Ptr& oNumSkillPoints);

    bool loadState(
      const std::string& filename,
      const osg::ref_ptr<ModelContainer>& modelContainer,
      const osgGaming::Observable<int>::Ptr& oDay,
      const osgGaming::Observable<int>::Ptr& oNumSkillPoints,
      UpdateThread* thread);

  };
}