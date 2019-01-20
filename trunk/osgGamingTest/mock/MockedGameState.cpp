#include "MockedGameState.h"

#include <osgGaming/View.h>
#include <osgGaming/World.h>

namespace osgGamingTest
{
  struct MockedState::Impl
  {
    std::function<StateEvent*(MockedState*, double)> func;
    unsigned char properties;
    osg::ref_ptr<osgGaming::World> overrideWorld;

    std::set<double> deliveredTicks;
  };

  MockedState::MockedState(
    std::function<StateEvent*(MockedState*, double)> func,
    unsigned char properties,
    osg::ref_ptr<osgGaming::World> overrideWorld)
    : osgGaming::AbstractGameState()
    , m(new Impl())
  {
    m->func = func;
    m->properties = properties;
    m->overrideWorld = overrideWorld;
  }

  MockedState::~MockedState()
  {
  }

  bool MockedState::deliveredTick(double tick)
  {
    return m->deliveredTicks.count(tick) > 0;
  }

  bool MockedState::isLoadingState()
  {
    return false;
  }

  osgGaming::AbstractGameState::StateEvent* MockedState::update()
  {
    double tick = getSimulationTime();

    m->deliveredTicks.insert(tick);

    return m->func(this, tick);
  }

  unsigned char MockedState::getProperties()
  {
    return m->properties;
  }

  osg::ref_ptr<osgGaming::World> MockedState::injectWorld(osgGaming::Injector& injector, osg::ref_ptr<osgGaming::View> view)
  {
    return m->overrideWorld;
  }

  osgGaming::AbstractGameState::StateEvent* MockedState::stateEvent_pop()
  {
    return osgGaming::AbstractGameState::stateEvent_pop();
  }

  osgGaming::AbstractGameState::StateEvent* MockedState::stateEvent_endGame()
  {
    return osgGaming::AbstractGameState::stateEvent_endGame();
  }
}