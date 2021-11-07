#include "GlobeCameraState.h"
#include "nodes/GlobeOverviewWorld.h"

#include "nodes/BackgroundModel.h"

#include <osgHelper/Helper.h>
#include <osgHelper/View.h>

#include <QPointer>

#include <QtUtilsLib/Macros.h>

namespace onep
{
  struct GlobeCameraState::Impl
  {
    explicit Impl(osgHelper::ioc::Injector& injector)
      : globeWorld(injector.inject<GlobeOverviewWorld>())
      , backgroundModel(injector.inject<BackgroundModel>())
      , cameraDistance(0.0f)
    {}

    osg::ref_ptr<GlobeOverviewWorld> globeWorld;
    osg::ref_ptr<BackgroundModel>    backgroundModel;
    osg::ref_ptr<osgHelper::View>    view;

    osg::Vec2f cameraLatLong;
    float      cameraDistance;
    osg::Vec2f cameraViewAngle;

    osg::ref_ptr<osgHelper::RepeatedVec2fAnimation> cameraLatLongAnimation;
    osg::ref_ptr<osgHelper::Animation<float>>       cameraDistanceAnimation;
    osg::ref_ptr<osgHelper::Animation<osg::Vec2f>>  cameraViewAngleAnimation;
  };

  GlobeCameraState::GlobeCameraState(osgHelper::ioc::Injector& injector)
    : EventProcessingState(injector)
    , m(new Impl(injector))
  {
  }

  GlobeCameraState::~GlobeCameraState() = default;

  void GlobeCameraState::onInitialize(QPointer<QtOsgBridge::MainWindow> mainWindow, const SimulationData& data)
  {
    m->view = mainWindow->getViewWidget()->getView();

    m->cameraLatLong   = m->globeWorld->getCameraLatLong();
    m->cameraDistance  = m->globeWorld->getCameraDistance();
    m->cameraViewAngle = m->globeWorld->getCameraViewAngle();

    m->cameraLatLongAnimation =
            new osgHelper::RepeatedVec2fAnimation(osg::Vec2f(-C_PI / 2.0f, 0.0f), osg::Vec2f(C_PI / 2.0f, 2.0f * C_PI),
                                                  m->cameraLatLong, 0.5, osgHelper::AnimationEase::CIRCLE_OUT);
    m->cameraDistanceAnimation =
            new osgHelper::Animation<float>(m->cameraDistance, 0.5, osgHelper::AnimationEase::CIRCLE_OUT);
    m->cameraViewAngleAnimation =
            new osgHelper::Animation<osg::Vec2f>(m->cameraViewAngle, 0.5, osgHelper::AnimationEase::CIRCLE_OUT);

    m->backgroundModel->updateResolutionHeight(static_cast<float>(m->view->getResolution().y()));
  }

  void GlobeCameraState::onUpdate(const SimulationData& data)
  {
    m->globeWorld->updateCamera(
            m->view->getCamera(osgHelper::View::CameraType::Scene), m->cameraLatLongAnimation->getValue(data.time),
            m->cameraViewAngleAnimation->getValue(data.time), m->cameraDistanceAnimation->getValue(data.time));
  }

  const osg::Vec2f& GlobeCameraState::getCameraLatLong() const
  {
    return m->cameraLatLong;
  }

  float GlobeCameraState::getCameraDistance() const
  {
    return m->cameraDistance;
  }

  const osg::Vec2f& GlobeCameraState::getCameraViewAngle() const
  {
    return m->cameraViewAngle;
  }

  void GlobeCameraState::setCameraLatLong(const osg::Vec2f& latLong, double time)
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

  void GlobeCameraState::setCameraMotionEase(osgHelper::AnimationEase ease)
  {
    m->cameraDistanceAnimation->setEase(ease);
    m->cameraLatLongAnimation->setEase(ease);
    m->cameraViewAngleAnimation->setEase(ease);
  }

  void GlobeCameraState::stopMotion(double time)
  {
    m->cameraDistanceAnimation->setValue(m->cameraDistanceAnimation->getValue(time));
    m->cameraLatLongAnimation->setValue(m->cameraLatLongAnimation->getValue(time));
    m->cameraViewAngleAnimation->setValue(m->cameraViewAngleAnimation->getValue(time));
  }

  void GlobeCameraState::setCameraViewAngle(const osg::Vec2f& viewAngle, double time)
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

  bool GlobeCameraState::isCameraInMotion() const
  {
    return m->cameraDistanceAnimation->running();
  }

}