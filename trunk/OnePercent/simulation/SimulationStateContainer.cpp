#include "SimulationStateContainer.h"

#include "simulation/SimulationState.h"

namespace onep
{
  struct SimulationStateContainer::Impl
  {
    Impl() {}

    SimulationState::Ptr state;
  };

  SimulationStateContainer::SimulationStateContainer(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl())
  {
  }

  SimulationStateContainer::~SimulationStateContainer()
  {
  }

  osg::ref_ptr<SimulationState> SimulationStateContainer::getState()
  {
    return m->state;
  }

  void SimulationStateContainer::loadFromLua(const luabridge::LuaRef object)
  {
    m->state = new SimulationState(object);
  }
}