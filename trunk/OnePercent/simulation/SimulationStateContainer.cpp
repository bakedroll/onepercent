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

  SimulationStateContainer::~SimulationStateContainer() = default;

  void SimulationStateContainer::accessState(std::function<void(std::shared_ptr<SimulationState>)> func)
  {
    QMutexLocker lock(&m->mutexState);
    func(m->state);
  }

  void SimulationStateContainer::loadFromLua(const luabridge::LuaRef object)
  {
    QMutexLocker lock(&m->mutexState);
    m->state.reset(new SimulationState(object));
  }
}