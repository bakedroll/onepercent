#pragma once

#include <osgGaming/AbstractGameState.h>

namespace osgGamingTest
{
  class MockedState : public osgGaming::AbstractGameState
  {
  public:
    MockedState(
      std::function<StateEvent*(MockedState*, double)> func,
      unsigned char properties = PROP_GUIEVENTS_TOP | PROP_UIMEVENTS_TOP | PROP_UPDATE_TOP,
      osg::ref_ptr<osgGaming::World> overrideWorld = nullptr);

    ~MockedState();

    bool deliveredTick(double tick);

    virtual bool isLoadingState() override;

    virtual StateEvent* update() override;
    virtual unsigned char getProperties() override;

    virtual osg::ref_ptr<osgGaming::World> overrideWorld() override;

    StateEvent* stateEvent_push(osg::ref_ptr<AbstractGameState> state);
    StateEvent* stateEvent_push(AbstractGameStateList states);
    StateEvent* stateEvent_pop();
    StateEvent* stateEvent_replace(osg::ref_ptr<AbstractGameState> state);
    StateEvent* stateEvent_replace(AbstractGameStateList states);
    StateEvent* stateEvent_endGame();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };
}