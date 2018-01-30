#pragma once

#include "osgGaming/GameApplication.h"

namespace osgGamingTest
{
  class MockedGameApplication : public osgGaming::GameApplication
  {
  public:
    MockedGameApplication();
    ~MockedGameApplication();

    virtual int run(osgGaming::GameStateStack::AbstractGameStateList initialStates) override;

  protected:
    virtual int mainloop() override;
    virtual osg::ref_ptr<osgGaming::InputManager> createInputManager(osg::ref_ptr<osgGaming::View> view) override;

  };
}