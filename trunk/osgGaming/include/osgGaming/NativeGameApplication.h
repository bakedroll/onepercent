#pragma once

#include "GameApplication.h"

namespace osgGaming
{
  class NativeGameApplication : public GameApplication
  {
  public:
    NativeGameApplication(osg::ref_ptr<View> view = nullptr);

  protected:
    virtual int mainloop() override;
    virtual osg::ref_ptr<InputManager> createInputManager(osg::ref_ptr<osgGaming::View> view) override;

  };
}