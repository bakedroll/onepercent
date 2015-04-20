#pragma once

#include <osgGaming/GameState.h>
#include <osgGaming/World.h>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>

namespace osgGaming
{
	class InputManager : public osgGA::GUIEventHandler
	{
	public:
		InputManager(osg::ref_ptr<World> world, osg::ref_ptr<World> worldLoading);

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&) override;

		void setGraphicsWindow(osg::ref_ptr<osgViewer::GraphicsWindow> graphicsWindow);
		void setCurrentState(osg::ref_ptr<GameState> state);

		void updateResolution(unsigned int width, unsigned int height);

	private:
		osg::ref_ptr<World> _world;
		osg::ref_ptr<World> _worldLoading;

		osg::ref_ptr<osgViewer::GraphicsWindow> _graphicsWindow;
		osg::ref_ptr<GameState> _currentState;

		unsigned int _resolutionWidth;
		unsigned int _resolutionHeight;
	};
}