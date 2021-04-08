#pragma once

#include <QtOsgBridge/EventProcessingState.h>
#include <osgHelper/Animation.h>

namespace onep
{
	class GlobeCameraState : public QtOsgBridge::EventProcessingState
	{
	public:
	  GlobeCameraState(osgHelper::ioc::Injector& injector);
    virtual ~GlobeCameraState();

		void onInitialize(QPointer<QtOsgBridge::MainWindow> mainWindow) override;
		void onUpdate(const SimulationData& data) override;

	protected:
		const osg::Vec2f& getCameraLatLong() const;
		float getCameraDistance() const;
		const osg::Vec2f& getCameraViewAngle() const;

		void setCameraLatLong(const osg::Vec2f& latLong, double time = -1.0);
		void setCameraDistance(float distance, double time = -1.0);
		void setCameraViewAngle(const osg::Vec2f& viewAngle, double time = -1.0);

		void setCameraMotionDuration(double time);
		void setCameraMotionEase(osgHelper::AnimationEase ease);

    void stopMotion(double time);

		bool isCameraInMotion() const;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
