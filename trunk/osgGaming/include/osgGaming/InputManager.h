#pragma once

#include <osg/Referenced>
#include <osgGA/GUIEventAdapter>
#include <memory>

namespace osgGaming
{
  class View;
  class GameStateStack;

	class InputManager : public osg::Referenced
	{
	public:
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