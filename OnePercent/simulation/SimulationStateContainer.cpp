#include "SimulationStateContainer.h"

#include "simulation/SimulationState.h"

#include <QMutex>

namespace onep
{
  struct SimulationStateContainer::Impl
  {
    Impl() {}

    QMutex mutexState;
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

  void SimulationStateContainer::accessState(std::function<void(osg::ref_ptr<SimulationState>)> func)
  {
    QMutexLocker lock(&m->mutexState);
    func(m->state);
  }

  void SimulationStateContainer::loadFromLua(const luabridge::LuaRef object)
  {
    QMutexLocker lock(&m->mutexState);
    m->state = new SimulationState(object);
  }
}