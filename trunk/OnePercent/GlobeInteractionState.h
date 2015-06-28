#pragma once

#include "GlobeOverviewWorld.h"
#include "GlobeCameraState.h"

#include <osgGaming/Animation.h>
#include <osgGaming/UIText.h>

#include <osgGaming/Timer.h>

namespace onep
{
	class GlobeInteractionState : public GlobeCameraState
	{
	public:
		GlobeInteractionState();

		virtual void initialize() override;

		virtual void onMousePressedEvent(int button, float x, float y) override;
		virtual void onKeyPressedEvent(int key) override;

		virtual void onScrollEvent(osgGA::GUIEventAdapter::ScrollingMotion motion) override;

		virtual void onDragEvent(int button, osg::Vec2f origin, osg::Vec2f position, osg::Vec2f change) override;
		virtual void onDragEndEvent(int button, osg::Vec2f origin, osg::Vec2f position) override;

		void dayTimerElapsed();

		void startSimulation();

	protected:
		virtual osg::ref_ptr<osgGaming::Hud> newHud() override;

	private:
		static const float _MIN_CAMERA_DISTANCE;
		static const float _MAX_CAMERA_DISTANCE;
		static const float _MAX_CAMERA_LONGITUDE;
		static const float _CAMERA_ZOOM_SPEED;
		static const float _CAMERA_ZOOM_SPEED_FACTOR;
		static const float _CAMERA_SCROLL_SPEED;
		static const float _CAMERA_ROTATION_SPEED;

		bool ready();

		osg::ref_ptr<osgGaming::UIText> _textPleaseSelect;
		osg::ref_ptr<osgGaming::UIText> _textConfirm;
		osg::ref_ptr<osgGaming::UIText> _textProgress;

		bool _ready;
		bool _started;

		int _selectedCountry;

		osg::ref_ptr<osgGaming::Timer> _simulationTimer;
	};
}