#include <gtest/gtest.h>

#include "mock/MockedGameApplication.h"
#include "mock/MockedGameState.h"
#include "mock/MockedWorld.h"

#include <osgGaming/GameState.h>


TEST(GameStateTest, UpdateProperties)
{
  osg::ref_ptr<osgGamingTest::MockedGameApplication> app = new osgGamingTest::MockedGameApplication();

  osgGaming::GameState::AbstractGameStateList states;
  states.push_back(new osgGamingTest::MockedState([](osgGamingTest::MockedState* state, double tick)
  {
    if (tick >= 3.0)
      return state->stateEvent_pop();

    return static_cast<osgGaming::AbstractGameState::StateEvent*>(nullptr);
  }, osgGaming::AbstractGameState::PROP_UPDATE_ALWAYS));

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

  app->run(states);

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
  osg::ref_ptr<osgGamingTest::MockedGameApplication> app = new osgGamingTest::MockedGameApplication();

  osg::ref_ptr<osgGamingTest::MockedWorld> world1 = new osgGamingTest::MockedWorld(1);
  osg::ref_ptr<osgGamingTest::MockedWorld> world2 = new osgGamingTest::MockedWorld(2);
  osg::ref_ptr<osgGamingTest::MockedWorld> world3 = new osgGamingTest::MockedWorld(3);

  app->setDefaultWorld(world1);

  osgGaming::GameState::AbstractGameStateList states;
  states.push_back(new osgGamingTest::MockedState([](osgGamingTest::MockedState* state, double tick)
  {
    return static_cast<osgGaming::AbstractGameState::StateEvent*>(nullptr);
  }, osgGaming::AbstractGameState::PROP_UPDATE_ALWAYS, nullptr));

  states.push_back(new osgGamingTest::MockedState([](osgGamingTest::MockedState* state, double tick)
  {
    return static_cast<osgGaming::AbstractGameState::StateEvent*>(nullptr);
  }, osgGaming::AbstractGameState::PROP_UPDATE_ALWAYS, world2));

  states.push_back(new osgGamingTest::MockedState([](osgGamingTest::MockedState* state, double tick)
  {
    return static_cast<osgGaming::AbstractGameState::StateEvent*>(nullptr);
  }, osgGaming::AbstractGameState::PROP_UPDATE_ALWAYS, nullptr));

  states.push_back(new osgGamingTest::MockedState([](osgGamingTest::MockedState* state, double tick)
  {
    return state->stateEvent_endGame();
  }, osgGaming::AbstractGameState::PROP_UPDATE_ALWAYS, world3));

  app->run(states);

  osg::ref_ptr<osgGamingTest::MockedState> state0 = dynamic_cast<osgGamingTest::MockedState*>(states[0].get());
  osg::ref_ptr<osgGamingTest::MockedState> state1 = dynamic_cast<osgGamingTest::MockedState*>(states[1].get());
  osg::ref_ptr<osgGamingTest::MockedState> state2 = dynamic_cast<osgGamingTest::MockedState*>(states[2].get());
  osg::ref_ptr<osgGamingTest::MockedState> state3 = dynamic_cast<osgGamingTest::MockedState*>(states[3].get());

  ASSERT_EQ(state0->getWorld(), world1);
  ASSERT_EQ(state1->getWorld(), world2);
  ASSERT_EQ(state2->getWorld(), world2);
  ASSERT_EQ(state3->getWorld(), world3);
}