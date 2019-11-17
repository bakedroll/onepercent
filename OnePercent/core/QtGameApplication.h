#pragma once

#include "core/Multithreading.h"

#include <osgGaming/GameApplication.h>
#include <osgGaming/AbstractGameState.h>

#include <memory>

namespace onep
{
  class QtGameApplication : public Multithreading, public osgGaming::GameApplication
  {
  public:
    QtGameApplication(int& argc, char** argv);
    ~QtGameApplication();

    bool notify(QObject *receiver, QEvent *event) override;

  protected:
    void deinitialize() override;

    void newStateEvent(osg::ref_ptr<osgGaming::AbstractGameState> state) override;
    void stateAttachedEvent(osg::ref_ptr<osgGaming::AbstractGameState> state) override;

    int mainloop() override;
    osg::ref_ptr<osgGaming::InputManager> createInputManager(osg::ref_ptr<osgGaming::View> view) override;

    void onException(const std::string& message) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}
