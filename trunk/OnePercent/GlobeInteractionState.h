#pragma once

#include "GlobeOverviewWorld.h"
#include "GlobeCameraState.h"

#include <osgGaming/Animation.h>

namespace onep
{
	class GlobeInteractionState : public GlobeCameraState
	{
	public:
		GlobeInteractionState();

		virtual void initialize() override;

		virtual void onMousePressedEvent(int button, float x, float y) override;

		virtual void onScrollEvent(osgGA::GUIEventAdapter::ScrollingMotion motion) override;

		virtual void onDragEvent(int button, osg::Vec2f origin, osg::Vec2f position, osg::Vec2f change) override;
		virtual void onDragEndEvent(int button, osg::Vec2f origin, osg::Vec2f position) override;

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

	};
}