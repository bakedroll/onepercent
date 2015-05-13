#pragma once

#include <osgGaming/GameState.h>
#include <osgGaming/World.h>
#include <osgGA/GUIEventHandler>
#include <osgGaming/Viewer.h>

namespace osgGaming
{
	class InputManager : public osgGA::GUIEventHandler
	{
	public:
		InputManager();

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

		void setViewer(osg::ref_ptr<osgGaming::Viewer> viewer);
		void setCurrentState(osg::ref_ptr<GameState> state);
		void setCurrentWorld(osg::ref_ptr<World> world);

		void updateResolution();
		void updateResolution(unsigned int width, unsigned int height);

		float getResolutionWidth();
		float getResolutionHeight();

	private:
		int mousePressed();

		void handleUserInteractionMove(float x, float y);

		unsigned int log_x_2(int x);

		static const unsigned int _NUM_MOUSE_BUTTONS = 3;

		osg::ref_ptr<World> _currentWorld;

		osg::ref_ptr<osgGaming::Viewer> _viewer;
		osg::ref_ptr<GameState> _currentState;

		unsigned int _resolutionWidth;
		unsigned int _resolutionHeight;

		bool _mousePressed[_NUM_MOUSE_BUTTONS];
		int _mouseDragging;
		osg::Vec2f _dragOrigin;
		osg::Vec2f _lastDragPosition;

		osg::Vec2f _mousePosition;
	};
}