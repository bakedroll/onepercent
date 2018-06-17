#pragma once

#include <osgGaming/AbstractGameState.h>

namespace osgGamingTest
{
  class MockedState : public osgGaming::AbstractGameState
  {
  public:
    MockedState(
      std::function<StateEvent*(MockedState*, double)> func,
      unsigned char properties = PROP_GUIEVENTS_TOP | PROP_UPDATE_TOP,
      osg::ref_ptr<osgGaming::World> overrideWorld = nullptr);

    ~MockedState();

    bool deliveredTick(double tick);

    virtual bool isLoadingState() override;

    virtual StateEvent* update() override;
    virtual unsigned char getProperties() override;

    virtual osg::ref_ptr<osgGaming::World> injectWorld(osgGaming::Injector& injector, osg::ref_ptr<osgGaming::View> view) override;

    template<typename TState>
    StateEvent* stateEvent_push()
    {
      return osgGaming::AbstractGameState::stateEvent_push<TState>();
    }

    template<typename TState>
    StateEvent* stateEvent_replace()
    {
      return osgGaming::AbstractGameState::stateEvent_replace<TState>();
    }

    virtual StateEvent* stateEvent_pop() override;
    virtual StateEvent* stateEvent_endGame() override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };
}