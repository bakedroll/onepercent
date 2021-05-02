#include "GlobeOverviewWorld.h"
#include "GlobeModel.h"
#include "BackgroundModel.h"

#include "nodes/CountryNameOverlay.h"
#include "simulation/Simulation.h"

#include <osgHelper/Helper.h>

namespace onep
{
  struct GlobeOverviewWorld::Impl
  {
    Impl(osgHelper::ioc::Injector& injector, GlobeOverviewWorld* b)
      : base(b),
        configManager(injector.inject<LuaConfig>()),
        paramEarthRadius(0.0f),
        globeModel(injector.inject<GlobeModel>()),
        countryNameOverlay(injector.inject<CountryNameOverlay>()),
        backgroundModel(injector.inject<BackgroundModel>()),
        simulation(injector.inject<Simulation>()),
        cameraLatLong(osg::Vec2f(0.0f, 0.0f)),
        cameraViewAngle(osg::Vec2f(0.0f, 0.0f)),
        cameraDistance(28.0f)
    {
    }


    void updateSun(osg::Vec3f sunDirection, const osgHelper::Camera::Ptr& camera)
    {
      osg::Vec3f look =  camera->getLookDirection();
      // look to sun
      if (look * sunDirection < 0.0f)
      {
        float range = 0.3f;

        float dist = osgHelper::pointLineDistance(camera->getPosition(), sunDirection, osg::Vec3f(0.0f, 0.0f, 0.0f));
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

  GlobeOverviewWorld::GlobeOverviewWorld(osgHelper::ioc::Injector& injector)
    : osgHelper::LightingNode()
    , m(new Impl(injector, this))
  {
    getLightModel()->setAmbientIntensity(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

    setLightEnabled(0, true);

    osg::ref_ptr<osg::Light> light = getOrCreateLight(0);

    light->setDiffuse(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    light->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    light->setAmbient(osg::Vec4(0.0, 0.0, 0.0, 1.0));

    addChild(m->globeModel);
    addChild(m->countryNameOverlay);
    addChild(m->backgroundModel);
  }

  GlobeOverviewWorld::~GlobeOverviewWorld() = default;

  void GlobeOverviewWorld::initialize()
  {
    m->paramEarthRadius = m->configManager->getNumber<float>("earth.radius");
  }

  void GlobeOverviewWorld::setDay(const osgHelper::Camera::Ptr& camera, float day)
  {
    float daysInYear = m->configManager->getNumber<float>("mechanics.days_in_year");
    float year = day / daysInYear;

    osg::Matrix yearMat = osgHelper::getMatrixFromEuler(0.0f, 0.0f, -year * 2.0f * C_PI) *
      osgHelper::getMatrixFromEuler(-sin(year * 2.0f * C_PI) * 23.5f * C_PI / 180.0f, 0.0f, 0.0f);
    osg::Matrix dayMat = osgHelper::getMatrixFromEuler(0.0f, 0.0f, day * 2.0f * C_PI);

    osg::Matrix yearDayMat = dayMat * yearMat;

    osg::Vec3f direction = yearDayMat * osg::Vec3f(0.0f, 1.0f, 0.0f);

    getOrCreateLight(0)->setPosition(osg::Vec4f(direction, 0.0f));
    m->globeModel->updateLightDir(direction);

    m->backgroundModel->setAttitude(osg::Matrix::inverse(dayMat).getRotate());
    m->backgroundModel->getSunTransform()->setAttitude(osg::Matrix::inverse(yearMat).getRotate());

    m->globeModel->updateClouds(day);
    m->updateSun(direction, camera);
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

  void GlobeOverviewWorld::updateCamera(const osgHelper::Camera::Ptr& camera, osg::Vec2f latLong, osg::Vec2f viewAngle, float distance)
  {
    m->cameraLatLong = latLong;
    m->cameraViewAngle = viewAngle;
    m->cameraDistance = distance;

    // update camera m_position
    osg::Vec3f position = osgHelper::getVec3FromEuler(latLong.x(), 0.0, latLong.y()) * m->cameraDistance;

    camera->setPosition(position);

    m->backgroundModel->setPosition(position);

    // update camera attitude
    osg::Matrix latLongMat = osg::Matrix::rotate(osgHelper::getQuatFromEuler(-latLong.x(), 0.0f, fmodf(latLong.y() + C_PI, C_PI * 2.0f)));
    osg::Matrix viewAngleMat = osg::Matrix::rotate(osgHelper::getQuatFromEuler(viewAngle.y(), viewAngle.x(), 0.0f));

    osg::Matrix mat = viewAngleMat * latLongMat;

    camera->setAttitude(mat.getRotate());
  }

}