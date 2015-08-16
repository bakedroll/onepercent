#pragma once

#include "GlobeOverviewWorld.h"
#include "GlobeCameraState.h"
#include "Globals.h"

#include <osgGaming/Animation.h>
#include <osgGaming/UIText.h>
#include <osgGaming/Property.h>

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

		virtual void onUIClickedEvent(osg::ref_ptr<osgGaming::UIElement> uiElement) override;

		void dayTimerElapsed();

		void startSimulation();

	protected:
		virtual osg::ref_ptr<osgGaming::Hud> newHud() override;

	private:
		osgGaming::Property<float, Param_EarthRadiusName> _paramEarthRadius;

		osgGaming::Property<float, Param_CameraMinDistanceName> _paramCameraMinDistance;
		osgGaming::Property<float, Param_CameraMaxDistanceName> _paramCameraMaxDistance;
		osgGaming::Property<float, Param_CameraMaxLatitudeName> _paramCameraMaxLatitude;
		osgGaming::Property<float, Param_CameraZoomSpeedName> _paramCameraZoomSpeed;
		osgGaming::Property<float, Param_CameraZoomSpeedFactorName> _paramCameraZoomSpeedFactor;
		osgGaming::Property<float, Param_CameraScrollSpeedName> _paramCameraScrollSpeed;
		osgGaming::Property<float, Param_CameraRotationSpeedName> _paramCameraRotationSpeed;

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