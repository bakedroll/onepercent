#include "GlobeOverviewWorld.h"
#include "GlobeModel.h"
#include "BackgroundModel.h"

#include <osgGaming/Helper.h>
#include <osgGaming/PropertiesManager.h>

namespace onep
{
  struct GlobeOverviewWorld::Impl
  {
    Impl(GlobeOverviewWorld* b) 
      : base(b)
      , paramEarthRadius(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_EarthRadiusName))
      , simulation(new Simulation())
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

    float paramEarthRadius;

    osg::ref_ptr<GlobeModel> globeModel;
    osg::ref_ptr<CountryNameOverlay> countryNameOverlay;
    osg::ref_ptr<BackgroundModel> backgroundModel;

    osg::ref_ptr<Simulation> simulation;

    osg::Vec2f cameraLatLong;
    osg::Vec2f cameraViewAngle;
    float cameraDistance;
  };

  GlobeOverviewWorld::GlobeOverviewWorld()
    : World()
    , m(new Impl(this))
  {
    getGlobalLightModel()->setAmbientIntensity(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

    setLightEnabled(0, true);

    osg::ref_ptr<osg::Light> light = getLight(0);

    light->setDiffuse(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    light->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    light->setAmbient(osg::Vec4(0.0, 0.0, 0.0, 1.0));
  }

  GlobeOverviewWorld::~GlobeOverviewWorld()
  {
  }

  osg::ref_ptr<GlobeModel> GlobeOverviewWorld::getGlobeModel()
  {
    return m->globeModel;
  }

  osg::ref_ptr<CountryNameOverlay> GlobeOverviewWorld::getCountryOverlay()
  {
    return m->countryNameOverlay;
  }

  osg::ref_ptr<BackgroundModel> GlobeOverviewWorld::getBackgroundModel()
  {
    return m->backgroundModel;
  }

  osg::ref_ptr<Simulation> GlobeOverviewWorld::getSimulation()
  {
    return m->simulation;
  }

  void GlobeOverviewWorld::setGlobeModel(osg::ref_ptr<GlobeModel> globeModel)
  {
    if (m->globeModel.valid())
    {
      getRootNode()->removeChild(m->globeModel);
    }

    getRootNode()->addChild(globeModel);
    m->globeModel = globeModel;
  }

  void GlobeOverviewWorld::setCountryNameOverlay(osg::ref_ptr<CountryNameOverlay> countryNameOverlay)
  {
    if (m->countryNameOverlay.valid())
    {
      getRootNode()->removeChild(m->countryNameOverlay);
    }

    getRootNode()->addChild(countryNameOverlay);
    m->countryNameOverlay = countryNameOverlay;
  }

  void GlobeOverviewWorld::setBackgroundModel(osg::ref_ptr<BackgroundModel> backgroundModel)
  {
    if (m->backgroundModel.valid())
    {
      getRootNode()->removeChild(m->backgroundModel->getTransform());
    }

    getRootNode()->addChild(backgroundModel->getTransform());
    m->backgroundModel = backgroundModel;
  }

  void GlobeOverviewWorld::setDay(float day)
  {
    float daysInYear = osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_MechanicsDaysInYearName);
    float year = day / daysInYear;

    osg::Matrix yearMat = osgGaming::getMatrixFromEuler(0.0f, 0.0f, -year * 2.0f * C_PI) *
      osgGaming::getMatrixFromEuler(-sin(year * 2.0f * C_PI) * 23.5f * C_PI / 180.0f, 0.0f, 0.0f);
    osg::Matrix dayMat = osgGaming::getMatrixFromEuler(0.0f, 0.0f, day * 2.0f * C_PI);

    osg::Matrix yearDayMat = dayMat * yearMat;

    osg::Vec3f direction = yearDayMat * osg::Vec3f(0.0f, 1.0f, 0.0f);

    getLight(0)->setPosition(osg::Vec4f(direction, 0.0f));
    m->globeModel->updateLightDirection(direction);

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