#pragma once

#include <osg/Referenced>
#include <osgGA/GUIEventAdapter>
#include <osgGaming/View.h>

#include <memory>

namespace osgGaming
{
  class NativeView;
  class GameStateStack;

	class InputManager : public osg::Referenced
	{
	public:
    typedef osg::ref_ptr<InputManager> Ptr;

		InputManager();
    ~InputManager();

    void setView(osg::ref_ptr<osgGaming::View> viewer);
		void setGameStateStack(GameStateStack* stack);

		void updateNewRunningStates();

    void setIsInizialized(bool initialized);
    bool isInitialized();

  protected:
    bool onKeyPressEvent(const osgGA::GUIEventAdapter& ea);
    bool onKeyReleaseEvent(const osgGA::GUIEventAdapter& ea);
    bool onMouseClickEvent(const osgGA::GUIEventAdapter& ea);
    bool onMouseReleaseEvent(const osgGA::GUIEventAdapter& ea);
    bool onMouseMoveEvent(const osgGA::GUIEventAdapter& ea);
    bool onMouseScrollEvent(const osgGA::GUIEventAdapter& ea);
    bool onResizeEvent(const osgGA::GUIEventAdapter& ea);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}