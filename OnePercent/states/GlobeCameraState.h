#pragma once

#include "nodes/GlobeOverviewWorld.h"
#include "states/QtGameState.h"

#include <osgGaming/Animation.h>

namespace onep
{
	class GlobeCameraState : public QtGameState
	{
	public:
	  GlobeCameraState(osgGaming::Injector& injector);
    virtual ~GlobeCameraState();

		virtual void initialize() override;

		virtual osgGaming::GameState::StateEvent* update() override;

	protected:
		osg::Vec2f getCameraLatLong();
		float getCameraDistance();
		osg::Vec2f getCameraViewAngle();

		void setCameraLatLong(osg::Vec2f latLong, double time = -1.0);
		void setCameraDistance(float distance, double time = -1.0);
		void setCameraViewAngle(osg::Vec2f viewAngle, double time = -1.0);

		void setCameraMotionDuration(double time);
		void setCameraMotionEase(osgGaming::AnimationEase ease);

    void stopMotion(double time);

		bool isCameraInMotion();

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
