#include "SimulationStateContainer.h"

#include "scripting/LuaSimulationState.h"

#include <QMutex>

namespace onep
{
  struct SimulationStateContainer::Impl
  {
    Impl() {}

    QMutex mutexState;
    LuaSimulationState::Ptr state;
  };

  SimulationStateContainer::SimulationStateContainer(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl())
  {
  }

  SimulationStateContainer::~SimulationStateContainer() = default;

  void SimulationStateContainer::accessState(std::function<void(std::shared_ptr<LuaSimulationState>)> func)
  {
    QMutexLocker lock(&m->mutexState);
    func(m->state);
  }

  void SimulationStateContainer::loadFromLua(const luabridge::LuaRef object)
  {
    QMutexLocker lock(&m->mutexState);
    m->state.reset(new LuaSimulationState(object));
  }
}