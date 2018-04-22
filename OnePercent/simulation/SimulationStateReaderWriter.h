#pragma once

#include <string>

#include <osg/ref_ptr>

#include <osgGaming/Observable.h>

namespace onep
{
  class SimulationStateContainer;
  class SkillsContainer;
  class UpdateThread;

  class SimulationStateReaderWriter
  {
  public:
    SimulationStateReaderWriter();
    ~SimulationStateReaderWriter();

    bool saveState(
      const std::string& filename,
      osg::ref_ptr<SimulationStateContainer> stateContainer,
      osg::ref_ptr<SkillsContainer> skillsContainer,
      osgGaming::Observable<int>::Ptr oDay,
      osgGaming::Observable<int>::Ptr oNumSkillPoints);

    bool loadState(
      const std::string& filename,
      osg::ref_ptr<SimulationStateContainer> stateContainer,
      osg::ref_ptr<SkillsContainer> skillsContainer,
      osgGaming::Observable<int>::Ptr oDay,
      osgGaming::Observable<int>::Ptr oNumSkillPoints,
      UpdateThread* thread);

  };
}