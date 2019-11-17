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

		void initialize() override;

		StateEvent* update() override;

	protected:
		const osg::Vec2f& getCameraLatLong() const;
		float getCameraDistance() const;
		const osg::Vec2f& getCameraViewAngle() const;

		void setCameraLatLong(const osg::Vec2f& latLong, double time = -1.0);
		void setCameraDistance(float distance, double time = -1.0);
		void setCameraViewAngle(const osg::Vec2f& viewAngle, double time = -1.0);

		void setCameraMotionDuration(double time);
		void setCameraMotionEase(osgGaming::AnimationEase ease);

    void stopMotion(double time);

		bool isCameraInMotion() const;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
