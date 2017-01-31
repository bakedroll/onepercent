#include <osgGaming/NativeInputManager.h>

namespace osgGaming
{
  NativeInputManager::NativeInputManager()
    : InputManager()
    , osgGA::GUIEventHandler()
  {
  }

  bool NativeInputManager::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
  {
    switch (ea.getEventType())
    {
    case osgGA::GUIEventAdapter::KEYDOWN:
    {
      return onKeyPressEvent(ea);
    }
    case osgGA::GUIEventAdapter::KEYUP:
    {
      return onKeyReleaseEvent(ea);
    }
    case osgGA::GUIEventAdapter::PUSH:
    case osgGA::GUIEventAdapter::DOUBLECLICK:
    {
      return onMouseClickEvent(ea);
    }
    case osgGA::GUIEventAdapter::RELEASE:
    {
      onMouseReleaseEvent(ea);
    }
    case osgGA::GUIEventAdapter::RESIZE:
    {
      return onResizeEvent(ea);
    }
    case osgGA::GUIEventAdapter::DRAG:
    case osgGA::GUIEventAdapter::MOVE:
    {
      onMouseMoveEvent(ea);
    }
    case osgGA::GUIEventAdapter::SCROLL:
    {
      onMouseScrollEvent(ea);
    }
    default: break;
    }

    return false;
  }

  osg::ref_ptr<osgGA::GUIEventHandler> NativeInputManager::handler()
  {
    return this;
  }
}