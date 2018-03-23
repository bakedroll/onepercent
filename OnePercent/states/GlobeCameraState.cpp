#include "GlobeCameraState.h"

#include <osgGaming/Helper.h>
#include <osgGaming/NativeView.h>

namespace onep
{
  struct GlobeCameraState::Impl
  {
    Impl(osgGaming::Injector& injector)
      : globeWorld(injector.inject<GlobeOverviewWorld>())
      , backgroundModel(injector.inject<BackgroundModel>())
      , cameraDistance(0.0f)
    {}

    osg::ref_ptr<GlobeOverviewWorld> globeWorld;
    osg::ref_ptr<BackgroundModel> backgroundModel;

    osg::Vec2f cameraLatLong;
    float cameraDistance;
    osg::Vec2f cameraViewAngle;

    osg::ref_ptr<osgGaming::RepeatedVec2fAnimation> cameraLatLongAnimation;
    osg::ref_ptr<osgGaming::Animation<float>> cameraDistanceAnimation;
    osg::ref_ptr<osgGaming::Animation<osg::Vec2f>> cameraViewAngleAnimation;
  };

  GlobeCameraState::GlobeCameraState(osgGaming::Injector& injector)
    : QtGameState()
    , m(new Impl(injector))
  {
  }

  GlobeCameraState::~GlobeCameraState()
  {
  }

  void GlobeCameraState::initialize()
  {
    m->cameraLatLong = m->globeWorld->getCameraLatLong();
    m->cameraDistance = m->globeWorld->getCameraDistance();
    m->cameraViewAngle = m->globeWorld->getCameraViewAngle();

    m->cameraLatLongAnimation = new osgGaming::RepeatedVec2fAnimation(osg::Vec2f(-C_PI / 2.0f, 0.0f), osg::Vec2f(C_PI / 2.0f, 2.0f * C_PI), m->cameraLatLong, 0.5, osgGaming::CIRCLE_OUT);
    m->cameraDistanceAnimation = new osgGaming::Animation<float>(m->cameraDistance, 0.5, osgGaming::CIRCLE_OUT);
    m->cameraViewAngleAnimation = new osgGaming::Animation<osg::Vec2f>(m->cameraViewAngle, 0.5, osgGaming::CIRCLE_OUT);

    m->backgroundModel->updateResolutionHeight(getView(0)->getResolution().y());
  }

  osgGaming::GameState::StateEvent* GlobeCameraState::update()
  {
    m->globeWorld->updateCameraPosition(
      m->cameraLatLongAnimation->getValue(getSimulationTime()),
      m->cameraViewAngleAnimation->getValue(getSimulationTime()),
      m->cameraDistanceAnimation->getValue(getSimulationTime()));

    return stateEvent_default();
  }

  osg::Vec2f GlobeCameraState::getCameraLatLong()
  {
    return m->cameraLatLong;
  }

  float GlobeCameraState::getCameraDistance()
  {
    return m->cameraDistance;
  }

  osg::Vec2f GlobeCameraState::getCameraViewAngle()
  {
    return m->cameraViewAngle;
  }

  void GlobeCameraState::setCameraLatLong(osg::Vec2f latLong, double time)
  {
    m->cameraLatLong = latLong;

    if (time < 0.0)
    {
      m->cameraLatLongAnimation->setValue(m->cameraLatLong);
    }
    else
    {
      m->cameraLatLongAnimation->beginAnimation(m->cameraLatLong, time);
    }
  }

  void GlobeCameraState::setCameraDistance(float distance, double time)
  {
    m->cameraDistance = distance;

    if (time < 0.0)
    {
      m->cameraDistanceAnimation->setValue(m->cameraDistance);
    }
    else
    {
      m->cameraDistanceAnimation->beginAnimation(m->cameraDistance, time);
    }
  }

  void GlobeCameraState::setCameraMotionDuration(double time)
  {
    m->cameraDistanceAnimation->setDuration(time);
    m->cameraLatLongAnimation->setDuration(time);
    m->cameraViewAngleAnimation->setDuration(time);
  }

  void GlobeCameraState::setCameraMotionEase(osgGaming::AnimationEase ease)
  {
    m->cameraDistanceAnimation->setEase(ease);
    m->cameraLatLongAnimation->setEase(ease);
    m->cameraViewAngleAnimation->setEase(ease);
  }

  void GlobeCameraState::setCameraViewAngle(osg::Vec2f viewAngle, double time)
  {
    m->cameraViewAngle = viewAngle;

    if (time < 0.0)
    {
      m->cameraViewAngleAnimation->setValue(m->cameraViewAngle);
    }
    else
    {
      m->cameraViewAngleAnimation->beginAnimation(m->cameraViewAngle, time);
    }
  }

  bool GlobeCameraState::isCameraInMotion()
  {
    return m->cameraDistanceAnimation->running();
  }

}