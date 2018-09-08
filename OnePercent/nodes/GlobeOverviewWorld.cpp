#include "GlobeOverviewWorld.h"
#include "GlobeModel.h"
#include "BackgroundModel.h"

#include "nodes/CountryNameOverlay.h"
#include "simulation/Simulation.h"

#include <osgGaming/Helper.h>

namespace onep
{
  struct GlobeOverviewWorld::Impl
  {
    Impl(osgGaming::Injector& injector, GlobeOverviewWorld* b)
      : base(b)
      , configManager(injector.inject<LuaConfig>())
      , globeModel(injector.inject<GlobeModel>())
      , countryNameOverlay(injector.inject<CountryNameOverlay>())
      , backgroundModel(injector.inject<BackgroundModel>())
      , simulation(injector.inject<Simulation>())
      , cameraLatLong(osg::Vec2f(0.0f, 0.0f))
      , cameraViewAngle(osg::Vec2f(0.0f, 0.0f))
      , cameraDistance(28.0f)
    {}


    void updateSun(osg::Vec3f sunDirection)
    {
      osg::Vec3f look = base->getCameraManipulator()->getLookDirection();
      // look to sun
      if (look * sunDirection < 0.0f)
      {
        float range = 0.3f;

        float dist = osgGaming::pointLineDistance(base->getCameraManipulator()->getPosition(), sunDirection, osg::Vec3f(0.0f, 0.0f, 0.0f));
        float scale = osg::clampBetween(dist - paramEarthRadius, 0.0f, range);
        scale /= range;
        scale *= 2.5f;

        backgroundModel->getSunGlowTransform()->setScale(osg::Vec3f(scale, scale, scale));
      }
    }

    GlobeOverviewWorld* base;

    osg::ref_ptr<LuaConfig> configManager;

    float paramEarthRadius;

    osg::ref_ptr<GlobeModel> globeModel;
    osg::ref_ptr<CountryNameOverlay> countryNameOverlay;
    osg::ref_ptr<BackgroundModel> backgroundModel;

    osg::ref_ptr<Simulation> simulation;

    osg::Vec2f cameraLatLong;
    osg::Vec2f cameraViewAngle;
    float cameraDistance;
  };

  GlobeOverviewWorld::GlobeOverviewWorld(osgGaming::Injector& injector)
    : World(injector)
    , m(new Impl(injector, this))
  {
    getGlobalLightModel()->setAmbientIntensity(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

    setLightEnabled(0, true);

    osg::ref_ptr<osg::Light> light = getLight(0);

    light->setDiffuse(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    light->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    light->setAmbient(osg::Vec4(0.0, 0.0, 0.0, 1.0));

    getRootNode()->addChild(m->globeModel);
    getRootNode()->addChild(m->countryNameOverlay);
    getRootNode()->addChild(m->backgroundModel->getTransform());
  }

  GlobeOverviewWorld::~GlobeOverviewWorld()
  {
  }

  void GlobeOverviewWorld::initialize()
  {
    m->paramEarthRadius = m->configManager->getNumber<float>("earth.radius");
  }

  void GlobeOverviewWorld::setDay(float day)
  {
    float daysInYear = m->configManager->getNumber<float>("mechanics.days_in_year");
    float year = day / daysInYear;

    osg::Matrix yearMat = osgGaming::getMatrixFromEuler(0.0f, 0.0f, -year * 2.0f * C_PI) *
      osgGaming::getMatrixFromEuler(-sin(year * 2.0f * C_PI) * 23.5f * C_PI / 180.0f, 0.0f, 0.0f);
    osg::Matrix dayMat = osgGaming::getMatrixFromEuler(0.0f, 0.0f, day * 2.0f * C_PI);

    osg::Matrix yearDayMat = dayMat * yearMat;

    osg::Vec3f direction = yearDayMat * osg::Vec3f(0.0f, 1.0f, 0.0f);

    getLight(0)->setPosition(osg::Vec4f(direction, 0.0f));
    m->globeModel->updateLightDir(direction);

    m->backgroundModel->getTransform()->setAttitude(osg::Matrix::inverse(dayMat).getRotate());
    m->backgroundModel->getSunTransform()->setAttitude(osg::Matrix::inverse(yearMat).getRotate());

    m->globeModel->updateClouds(day);
    m->updateSun(direction);
  }

  osg::Vec2f GlobeOverviewWorld::getCameraLatLong()
  {
    return m->cameraLatLong;
  }

  osg::Vec2f GlobeOverviewWorld::getCameraViewAngle()
  {
    return m->cameraViewAngle;
  }

  float GlobeOverviewWorld::getCameraDistance()
  {
    return m->cameraDistance;
  }

  void GlobeOverviewWorld::updateCameraPosition(osg::Vec2f latLong, osg::Vec2f viewAngle, float distance)
  {
    m->cameraLatLong = latLong;
    m->cameraViewAngle = viewAngle;
    m->cameraDistance = distance;

    // update camera position
    osg::Vec3f position = osgGaming::getVec3FromEuler(latLong.x(), 0.0, latLong.y()) * m->cameraDistance;

    getCameraManipulator()->setPosition(position);

    m->backgroundModel->getTransform()->setPosition(position);

    // update camera attitude
    osg::Matrix latLongMat = osg::Matrix::rotate(osgGaming::getQuatFromEuler(-latLong.x(), 0.0f, fmodf(latLong.y() + C_PI, C_PI * 2.0f)));
    osg::Matrix viewAngleMat = osg::Matrix::rotate(osgGaming::getQuatFromEuler(viewAngle.y(), viewAngle.x(), 0.0f));

    osg::Matrix mat = viewAngleMat * latLongMat;

    getCameraManipulator()->setAttitude(mat.getRotate());
  }

}