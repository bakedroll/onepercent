#include <gtest/gtest.h>

#include "mock/MockedGameApplication.h"
#include "mock/MockedGameState.h"
#include "mock/MockedWorld.h"

#include <osgGaming/GameState.h>

// Probably not needed anymore

/*
TEST(GameStateTest, UpdateProperties)
{
  osg::ref_ptr<osgGamingTest::MockedGameApplication> app = new osgGamingTest::MockedGameApplication();
  app->prepare();

  osgGaming::GameState::AbstractGameStateList states;
  states.push_back(new osgGamingTest::MockedState([](osgGamingTest::MockedState* state, double tick)
  {
    if (tick >= 3.0)
      return state->stateEvent_pop();

    return static_cast<osgGaming::AbstractGameState::StateEvent*>(nullptr);
  }, osgGaming::AbstractGameState::UpdateAlways));

  states.push_back(new osgGamingTest::MockedState([](osgGamingTest::MockedState* state, double tick)
  {
    return state->stateEvent_pop();
  }));

  states.push_back(new osgGamingTest::MockedState([](osgGamingTest::MockedState* state, double tick)
  {
    if (tick >= 1.0)
      return state->stateEvent_pop();

    return static_cast<osgGaming::AbstractGameState::StateEvent*>(nullptr);
  }));

  app->runGame(states);

  osg::ref_ptr<osgGamingTest::MockedState> state0 = dynamic_cast<osgGamingTest::MockedState*>(states[0].get());
  osg::ref_ptr<osgGamingTest::MockedState> state1 = dynamic_cast<osgGamingTest::MockedState*>(states[1].get());
  osg::ref_ptr<osgGamingTest::MockedState> state2 = dynamic_cast<osgGamingTest::MockedState*>(states[2].get());

  ASSERT_TRUE(state0->deliveredTick(0.0));
  ASSERT_TRUE(state0->deliveredTick(1.0));
  ASSERT_TRUE(state0->deliveredTick(2.0));
  ASSERT_TRUE(state0->deliveredTick(3.0));

  ASSERT_FALSE(state1->deliveredTick(0.0));
  ASSERT_FALSE(state1->deliveredTick(1.0));
  ASSERT_TRUE(state1->deliveredTick(2.0));
  ASSERT_FALSE(state1->deliveredTick(3.0));

  ASSERT_TRUE(state2->deliveredTick(0.0));
  ASSERT_TRUE(state2->deliveredTick(1.0));
  ASSERT_FALSE(state2->deliveredTick(2.0));
  ASSERT_FALSE(state2->deliveredTick(3.0));
}

TEST(GameStateTest, OverrideWorld)
{
  osgGaming::InjectionContainer container;
  osgGaming::Injector injector(container);

  container.registerType<osgGamingTest::MockedWorld>();

  osg::ref_ptr<osgGamingTest::MockedGameApplication> app = new osgGamingTest::MockedGameApplication();
  app->prepare();

  osg::ref_ptr<osgGamingTest::MockedWorld> world1 = injector.inject<osgGamingTest::MockedWorld>();
  osg::ref_ptr<osgGamingTest::MockedWorld> world2 = injector.inject<osgGamingTest::MockedWorld>();
  osg::ref_ptr<osgGamingTest::MockedWorld> world3 = injector.inject<osgGamingTest::MockedWorld>();

  world1->setId(1);
  world2->setId(2);
  world3->setId(3);

  app->setDefaultWorld(world1);

  osgGaming::GameState::AbstractGameStateList states;
  states.push_back(new osgGamingTest::MockedState([](osgGamingTest::MockedState* state, double tick)
  {
    return static_cast<osgGaming::AbstractGameState::StateEvent*>(nullptr);
  }, osgGaming::AbstractGameState::UpdateAlways, nullptr));

  states.push_back(new osgGamingTest::MockedState([](osgGamingTest::MockedState* state, double tick)
  {
    return static_cast<osgGaming::AbstractGameState::StateEvent*>(nullptr);
  }, osgGaming::AbstractGameState::UpdateAlways, world2));

  states.push_back(new osgGamingTest::MockedState([](osgGamingTest::MockedState* state, double tick)
  {
    return static_cast<osgGaming::AbstractGameState::StateEvent*>(nullptr);
  }, osgGaming::AbstractGameState::UpdateAlways, nullptr));

  states.push_back(new osgGamingTest::MockedState([](osgGamingTest::MockedState* state, double tick)
  {
    return state->stateEvent_endGame();
  }, osgGaming::AbstractGameState::UpdateAlways, world3));

  app->runGame(states);

  osg::ref_ptr<osgGamingTest::MockedState> state0 = dynamic_cast<osgGamingTest::MockedState*>(states[0].get());
  osg::ref_ptr<osgGamingTest::MockedState> state1 = dynamic_cast<osgGamingTest::MockedState*>(states[1].get());
  osg::ref_ptr<osgGamingTest::MockedState> state2 = dynamic_cast<osgGamingTest::MockedState*>(states[2].get());
  osg::ref_ptr<osgGamingTest::MockedState> state3 = dynamic_cast<osgGamingTest::MockedState*>(states[3].get());

  //ASSERT_EQ(state0->getWorld(app->getViewer()->getGamingView(0)), world1);
  //ASSERT_EQ(state1->getWorld(app->getViewer()->getGamingView(0)), world2);
  //ASSERT_EQ(state2->getWorld(app->getViewer()->getGamingView(0)), world2);
  //ASSERT_EQ(state3->getWorld(app->getViewer()->getGamingView(0)), world3);
}

*/