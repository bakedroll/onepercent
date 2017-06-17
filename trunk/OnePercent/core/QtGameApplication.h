#pragma once

#include <osgGaming/GameApplication.h>
#include <osgGaming/AbstractGameState.h>

#include <memory>

namespace onep
{
  class QtGameApplication : public osgGaming::GameApplication
  {
  public:
    QtGameApplication(int argc, char** argv);
    ~QtGameApplication();

  protected:
    virtual void stateAttachedEvent(osg::ref_ptr<osgGaming::AbstractGameState> state) override;

    virtual int mainloop() override;
    virtual osg::ref_ptr<osgGaming::InputManager> createInputManager(osg::ref_ptr<osgGaming::View> view) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}
