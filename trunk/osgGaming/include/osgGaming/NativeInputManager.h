#pragma once

#include "InputManager.h"

#include <osgGA/GUIEventHandler>

namespace osgGaming
{
  class NativeInputManager : public InputManager, protected osgGA::GUIEventHandler
  {
  public:
    NativeInputManager();

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

    osg::ref_ptr<osgGA::GUIEventHandler> handler();
  };
}