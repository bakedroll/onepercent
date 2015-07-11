#pragma once

#include "GlobeOverviewWorld.h"

#include <osgGaming/GameState.h>
#include <osgGaming/Animation.h>

namespace onep
{
	class GlobeCameraState : public osgGaming::GameState
	{
	public:
		GlobeCameraState();

		virtual void initialize() override;

		virtual osgGaming::GameState::StateEvent* update() override;

	protected:
		osg::ref_ptr<GlobeOverviewWorld> getGlobeOverviewWorld();

		osg::Vec2f getCameraLatLong();
		float getCameraDistance();
		osg::Vec2f getCameraViewAngle();

		void setCameraLatLong(osg::Vec2f latLong, double time = -1.0);
		void setCameraDistance(float distance, double time = -1.0);
		void setCameraViewAngle(osg::Vec2f viewAngle, double time = -1.0);

		void setCameraMotionDuration(double time);
		void setCameraMotionEase(osgGaming::AnimationEase ease);

		bool isCameraInMotion();

	private:
		osg::ref_ptr<GlobeOverviewWorld> _globeWorld;

		osg::Vec2f _cameraLatLong;
		float _cameraDistance;
		osg::Vec2f _cameraViewAngle;

		osg::ref_ptr<osgGaming::RepeatedVec2fAnimation> _cameraLatLongAnimation;
		osg::ref_ptr<osgGaming::Animation<float>> _cameraDistanceAnimation;
		osg::ref_ptr<osgGaming::Animation<osg::Vec2f>> _cameraViewAngleAnimation;

	};
}