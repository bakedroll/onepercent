#pragma once

#include "osgGaming/GameApplication.h"

namespace osgGamingTest
{
  class MockedGameApplication : public osgGaming::GameApplication
  {
  public:
    MockedGameApplication();
    ~MockedGameApplication();

    int runGame(osgGaming::AbstractGameState::AbstractGameStateRefList initialStates) override;

  protected:
    int mainloop() override;
    osg::ref_ptr<osgGaming::InputManager> createInputManager(osg::ref_ptr<osgGaming::View> view) override;

  };
}