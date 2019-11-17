#pragma once

#include <osgGaming/AbstractGameState.h>
#include <osgGaming/Helper.h>

namespace osgGamingTest
{
  class MockedState : public osgGaming::AbstractGameState
  {
  public:
    MockedState(
      const std::function<StateEvent*(MockedState*, double)>& func,
      unsigned char properties = underlying(StateProperties::PROP_GUIEVENTS_TOP) |
		                             underlying(StateProperties::PROP_UPDATE_TOP),
      const osg::ref_ptr<osgGaming::World>& overrideWorld = nullptr);

    ~MockedState();

    bool deliveredTick(double tick);

    bool isLoadingState() const override;

    StateEvent* update() override;
    unsigned char getProperties() const override;

    osg::ref_ptr<osgGaming::World> injectWorld(osgGaming::Injector& injector, const osg::ref_ptr<osgGaming::View>& view) override;

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

    StateEvent* stateEvent_pop() override;
    StateEvent* stateEvent_endGame() override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}