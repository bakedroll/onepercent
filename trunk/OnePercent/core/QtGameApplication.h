#pragma once

#include "core/Multithreading.h"

#include <osgGaming/GameApplication.h>
#include <osgGaming/AbstractGameState.h>

#include <memory>

namespace onep
{
  class QtGameApplication : public osgGaming::GameApplication
  {
  public:
    QtGameApplication(int& argc, char** argv);
    ~QtGameApplication();

  protected:
    virtual void deinitialize() override;

    virtual void newStateEvent(osg::ref_ptr<osgGaming::AbstractGameState> state) override;
    virtual void stateAttachedEvent(osg::ref_ptr<osgGaming::AbstractGameState> state) override;

    virtual int mainloop() override;
    virtual osg::ref_ptr<osgGaming::InputManager> createInputManager(osg::ref_ptr<osgGaming::View> view) override;

    std::shared_ptr<Multithreading> qApplication();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}
