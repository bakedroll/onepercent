#pragma once

#include <osgGaming/AbstractGameState.h>
#include <osgGaming/World.h>
#include <osgGaming/Hud.h>
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
		void setCurrentState(osg::ref_ptr<AbstractGameState> state);

	private:
		void updateResolution(osg::Vec2f resolution);

		int mousePressed();

		void handleUserInteractionMove(osg::ref_ptr<AbstractGameState> state, float x, float y);

		unsigned int log_x_2(int x);

		static const unsigned int _NUM_MOUSE_BUTTONS = 3;

		osg::ref_ptr<osgGaming::Viewer> _viewer;
		osg::ref_ptr<AbstractGameState> _currentState;

		bool _mousePressed[_NUM_MOUSE_BUTTONS];
		int _mouseDragging;
		osg::Vec2f _dragOrigin;
		osg::Vec2f _lastDragPosition;

		osg::Vec2f _mousePosition;
	};
}