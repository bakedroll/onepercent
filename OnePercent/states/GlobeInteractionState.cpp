#include "GlobeInteractionState.h"

#include "core/Macros.h"
#include "nodes/GlobeOverviewWorld.h"
#include "nodes/CountryOverlay.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaCountry.h"
#include "scripting/LuaCountriesTable.h"
#include "simulation/Simulation.h"
#include "simulation/ModelContainer.h"
#include "widgets/OverlayCompositor.h"
#include "widgets/VirtualOverlay.h"
#include "widgets/CountryMenuWidget.h"
#include "widgets/MainFrameWidget.h"
#include "widgets/DebugWindow.h"

#include <osgGA/GUIEventAdapter>

#include <osgGaming/Helper.h>
#include <osgGaming/Hud.h>
#include <osgGaming/View.h>

namespace onep
{
  const auto CountryNone = 0;

  struct GlobeInteractionState::Impl
  {
    Impl(osgGaming::Injector& injector, GlobeInteractionState* b)
      : base(b),
        pInjector(&injector),
        configManager(injector.inject<LuaConfig>()),
        globeOverviewWorld(injector.inject<GlobeOverviewWorld>()),
        simulation(injector.inject<Simulation>()),
        countryOverlay(injector.inject<CountryOverlay>()),
        modelContainer(injector.inject<ModelContainer>()),
        paramEarthRadius(0.0f),
        paramCameraMinDistance(0.0f),
        paramCameraMaxDistance(0.0f),
        paramCameraMaxLatitude(0.0f),
        paramCameraZoomSpeed(0.0f),
        paramCameraZoomSpeedFactor(0.0f),
        paramCameraScrollSpeed(0.0f),
        paramCameraRotationSpeed(0.0f),
        bReady(false),
        isInteractionEnabled(true),
        selectedCountry(0),
        hoveredCountry(0),
        countryMenuWidget(new CountryMenuWidget(injector)),
        mainFrameWidget(nullptr),
        debugWindow(nullptr),
        countryMenuWidgetFadeInAnimation(
          new osgGaming::Animation<float>(0.0f, 0.4f, osgGaming::AnimationEase::CIRCLE_IN)),
        countryMenuWidgetFadeOutAnimation(
          new osgGaming::Animation<float>(0.0f, 0.4f, osgGaming::AnimationEase::CIRCLE_OUT)),
        bFadingOutCountryMenu(false),
        bDraggingMidMouse(false),
        bFastAnimation(false)
    {
    }

    GlobeInteractionState* base;

    // only for debug window
    osgGaming::Injector* pInjector;

    osg::ref_ptr<LuaConfig> configManager;
    osg::ref_ptr<GlobeOverviewWorld> globeOverviewWorld;
    osg::ref_ptr<Simulation> simulation;
    osg::ref_ptr<CountryOverlay> countryOverlay;

    osg::ref_ptr<ModelContainer> modelContainer;

    float paramEarthRadius;
    float paramCameraMinDistance;
    float paramCameraMaxDistance;
    float paramCameraMaxLatitude;
    float paramCameraZoomSpeed;
    float paramCameraZoomSpeedFactor;
    float paramCameraScrollSpeed;
    float paramCameraRotationSpeed;

    bool bReady;
    bool isInteractionEnabled;

    int selectedCountry;
    int hoveredCountry;

    osgGaming::Observer<int>::Ptr selectedCountryObserver;

    CountryMenuWidget* countryMenuWidget;
    MainFrameWidget* mainFrameWidget;
    DebugWindow* debugWindow;

    osg::ref_ptr<osgGaming::Animation<float>> countryMenuWidgetFadeInAnimation;
    osg::ref_ptr<osgGaming::Animation<float>> countryMenuWidgetFadeOutAnimation;

    osg::Vec2i mousePos;

    bool bFadingOutCountryMenu;
    bool bDraggingMidMouse;
    bool bFastAnimation;

    float getViewAngleForDistance(float distance) const
    {
      float x = (paramCameraMaxDistance - distance) / (paramCameraMaxDistance - paramCameraMinDistance);
      return std::pow(x, 6.0f) * C_PI * 0.25f;
    }

    void setCameraDistanceAndAngle(float distance, float time = -1.0)
    {
      if (!bDraggingMidMouse)
        base->setCameraViewAngle(osg::Vec2f(0.0f, getViewAngleForDistance(distance)), time);
      base->setCameraDistance(distance, time);
    }

    int pickCountryIdAt(const osg::Vec2i& pos)
    {
      osg::Vec3f point, direction, pickResult;
      base->getWorld(base->getView(0))->getCameraManipulator()->getPickRay(float(pos.x()), float(pos.y()), point, direction);

      if (osgGaming::sphereLineIntersection(osg::Vec3f(0.0f, 0.0f, 0.0f), paramEarthRadius, point, direction, pickResult))
      {
        osg::Vec2f polar = osgGaming::getPolarFromCartesian(pickResult);
        return countryOverlay->getCountryId(polar);
      }

      return 0;
    }

    void setFastCameraMotion()
    {
      if (bFastAnimation)
        return;

      bFastAnimation = true;
      base->stopMotion(base->getSimulationTime());
      base->setCameraMotionDuration(0.5);
    }

    void setSlowCameraMotion()
    {
      base->setCameraMotionDuration(1.0);
      bFastAnimation = false;
    }

    bool ready()
    {
      if (!bReady && !base->isCameraInMotion())
      {
        setFastCameraMotion();
        base->setCameraMotionEase(osgGaming::AnimationEase::CIRCLE_OUT);
        bReady = true;
      }

      return bReady;
    }

    void updateCountryMenuWidgetPosition(int id)
    {
      osg::Vec2f latLong = countryOverlay->getCountryPresenter(id)->getCenterLatLong();

      osg::Vec3f position = osgGaming::getVec3FromEuler(latLong.x(), 0.0, latLong.y()) * paramEarthRadius;

      osg::ref_ptr<osg::Camera> cam = base->getView(0)->getSceneCamera();
      osg::Matrix win = cam->getViewport()->computeWindowMatrix();
      osg::Matrix view = globeOverviewWorld->getCameraManipulator()->getViewMatrix();
      osg::Matrix proj = cam->getProjectionMatrix();

      osg::Vec3 screen = position * view * proj * win;

      float alpha;
      if (bFadingOutCountryMenu)
        alpha = countryMenuWidgetFadeOutAnimation->getValue(base->getSimulationTime());
      else
        alpha = countryMenuWidgetFadeInAnimation->getValue(base->getSimulationTime());

      countryMenuWidget->setCenterPosition(int(screen.x()), int(base->getView(0)->getResolution().y() - screen.y()));
      countryMenuWidget->setColor(osg::Vec4f(1.0f, 1.0f, 1.0f, alpha));

      if (bFadingOutCountryMenu && alpha == 0.0f)
        countryMenuWidget->setVisible(false);
    }
  };

  GlobeInteractionState::GlobeInteractionState(osgGaming::Injector& injector)
    : GlobeCameraState(injector)
    , m(new Impl(injector, this))
  {
  }

  GlobeInteractionState::~GlobeInteractionState()
  {

  }

  void GlobeInteractionState::initialize()
  {
    GlobeCameraState::initialize();

    m->paramEarthRadius = m->configManager->getNumber<float>("earth.radius");
    m->paramCameraMinDistance = m->configManager->getNumber<float>("camera.min_distance");
    m->paramCameraMaxDistance = m->configManager->getNumber<float>("camera.max_distance");
    m->paramCameraMaxLatitude = m->configManager->getNumber<float>("camera.max_latitude");
    m->paramCameraZoomSpeed = m->configManager->getNumber<float>("camera.zoom_speed");
    m->paramCameraZoomSpeedFactor = m->configManager->getNumber<float>("camera.zoom_speed_factor");
    m->paramCameraScrollSpeed = m->configManager->getNumber<float>("camera.scroll_speed");
    m->paramCameraRotationSpeed = m->configManager->getNumber<float>("camera.rotation_speed");

    const auto& presenters  = m->countryOverlay->getCountryPresenters();
    auto        projAngle   = static_cast<float>(getWorld(getView(0))->getCameraManipulator()->getProjectionAngle());
    auto        projRatio   = static_cast<float>(getWorld(getView(0))->getCameraManipulator()->getProjectionRatio());
    auto        minDistance = std::numeric_limits<float>::max();
    auto        maxDistance = std::numeric_limits<float>::min();

    for (const auto& presenter : presenters)
    {
      auto dist = presenter.second->getOptimalCameraDistance(projAngle, projRatio);
      minDistance = std::min<float>(dist, minDistance);
      maxDistance = std::max<float>(dist, maxDistance);
    }

    m->selectedCountryObserver = m->countryOverlay->getOSelectedCountryId()->connect([this, minDistance, maxDistance](int id)
    {
      if (id > 0)
      {
        auto time = getSimulationTime();
        auto r = m->paramEarthRadius;

        auto countryPresenter = m->countryOverlay->getCountryPresenter(id);
        auto country          = m->modelContainer->getModel()->getCountriesTable()->getCountryById(id);

        assert_return(country);
        
        OSGG_QLOG_INFO(QString("Selected country (%1): %2").arg(country->getId()).arg(QString::fromLocal8Bit(country->getName().c_str())));

        float viewDistance = countryPresenter->getOptimalCameraDistance(
          float(getWorld(getView(0))->getCameraManipulator()->getProjectionAngle()),
          float(getWorld(getView(0))->getCameraManipulator()->getProjectionRatio()));

        viewDistance = std::max(1.3f, viewDistance); // minimal distance before clipping occurs

        float x = osg::clampBetween<float>((maxDistance - viewDistance) / (maxDistance - minDistance), 0.0f, 1.0f);
        float a = std::pow(x, 6.0f) * C_PI * 0.3f;
        float cosa = cos(a);

        float t = (cosa * cosa - 1.0f) * viewDistance * viewDistance + r * r;
        float dist1 = sqrt(t) + cosa * viewDistance - r;
        float dist2 = -sqrt(t) + cosa * viewDistance - r;
        float dist = t >= 0.0f ? std::max(dist1, dist2) : cosa * viewDistance - r;
        float latitudeShift = acos(osg::clampBetween<float>((r * r + (r + dist) * (r + dist) - viewDistance * viewDistance) / (2.0f * r * (r + dist)), 0.0f, 1.0f));

        osg::Vec2f latLong = countryPresenter->getCenterLatLong();
        /*
        OSGG_QLOG_DEBUG(QString("Min: %1 Max: %2 ViewDistance: %3 x: %9 Angle: %4 Distance: %5 %10 %11 Shift: %6 Lat: %7 cosa: %8")
          .arg(minDistance)
          .arg(maxDistance)
          .arg(viewDistance)
          .arg(a)
          .arg(dist)
          .arg(latitudeShift)
          .arg(latLong.x())
          .arg(cosa)
          .arg(x)
          .arg(dist1)
          .arg(dist2));
          */

        m->setSlowCameraMotion();
        setCameraLatLong(osg::Vec2f(latLong.x() - latitudeShift, latLong.y()), time);
        setCameraDistance(r + dist, time);
        setCameraViewAngle(osg::Vec2f(0.0f, a), time);

        if (m->selectedCountry != id || m->bFadingOutCountryMenu)
        {
          m->bFadingOutCountryMenu = false;
          m->countryMenuWidget->setVisible(true);

          if (m->selectedCountry != id)
          {
            m->countryMenuWidgetFadeInAnimation->beginAnimation(0.0f, 0.8f, time);
            m->countryMenuWidget->setCountry(country);
            m->updateCountryMenuWidgetPosition(id);
          }
          else
          {
            m->countryMenuWidgetFadeInAnimation->beginAnimation(m->countryMenuWidgetFadeOutAnimation->getValue(time), 0.8f, time);
          }
        }
      }
      else
      {
        m->countryMenuWidget->setVisible(false);
      }

      m->selectedCountry = id;
    });

    setCameraMotionDuration(2.0);
    setCameraMotionEase(osgGaming::AnimationEase::SMOOTHER);

    m->setCameraDistanceAndAngle(m->paramCameraMaxDistance, getSimulationTime());

    setupUi();

    QObject::connect(m->mainFrameWidget, &MainFrameWidget::toggledWidgetEnabled, [this](bool enabled)
    {
      m->isInteractionEnabled = !enabled;
      if (!m->isInteractionEnabled)
      {
        m->countryOverlay->setSelectedCountry(CountryNone);
      }
    });
  }

  osgGaming::AbstractGameState::StateEvent* GlobeInteractionState::update()
  {
    StateEvent* e = GlobeCameraState::update();

    if (m->selectedCountry > 0 && m->countryMenuWidget->isVisible())
      m->updateCountryMenuWidgetPosition(m->selectedCountry);

    int id = m->isInteractionEnabled ? m->pickCountryIdAt(m->mousePos) : CountryNone;
    if (id != m->hoveredCountry)
    {
      m->hoveredCountry = id;

      if (!m->bDraggingMidMouse)
        m->countryOverlay->setHoveredCountryId(id);
    }

    return e;
  }

  void GlobeInteractionState::onMousePressedEvent(int button, float x, float y)
  {
    if (!m->ready() || !m->isInteractionEnabled)
    {
      return;
    }

    if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
    {
      int selected = m->pickCountryIdAt(m->mousePos);
      m->countryOverlay->setSelectedCountry(selected);
    }
  }

  void GlobeInteractionState::onKeyPressedEvent(int key)
  {
    if (key == osgGA::GUIEventAdapter::KEY_Space)
    {
      if (m->simulation->isRunning())
      {
        m->simulation->stop();

        OSGG_LOG_INFO("Simulation stopped");
      }
      else
      {
        m->simulation->start();

        OSGG_LOG_INFO("Simulation started");
      }
    }
  }

  void GlobeInteractionState::onMouseMoveEvent(float x, float y)
  {
    m->mousePos.set(x, y);
  }

  void GlobeInteractionState::onScrollEvent(osgGA::GUIEventAdapter::ScrollingMotion motion)
  {
    if (!m->ready() || !m->isInteractionEnabled)
    {
      return;
    }

    float distance = getCameraDistance();

    if (motion == osgGA::GUIEventAdapter::SCROLL_UP)
    {
      distance = distance * m->paramCameraZoomSpeed;
    }
    else if (motion == osgGA::GUIEventAdapter::SCROLL_DOWN)
    {
      distance = distance * (1.0f / m->paramCameraZoomSpeed);
    }
    else
    {
      return;
    }

    distance = osg::clampBetween<float>(distance, m->paramCameraMinDistance, m->paramCameraMaxDistance);

    m->setFastCameraMotion();
    m->setCameraDistanceAndAngle(distance, getSimulationTime());
  }

  void GlobeInteractionState::onDragBeginEvent(int button, const osg::Vec2f& position)
  {
    if (!m->isInteractionEnabled)
    {
      return;
    }

    if (button == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
    {
      m->bDraggingMidMouse = true;
      if (m->hoveredCountry != 0)
        m->countryOverlay->setHoveredCountryId(0);
    }
  }

  void GlobeInteractionState::onDragEvent(int button, const osg::Vec2f& origin, const osg::Vec2f& position, const osg::Vec2f& change)
  {
    if (!m->ready() || !m->isInteractionEnabled)
	  {
      return;
	  }

    auto latLong   = getCameraLatLong();
    auto distance  = getCameraDistance();
    auto viewAngle = getCameraViewAngle();

    if (button != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
    {
      if (m->selectedCountry > 0 && !m->bFadingOutCountryMenu)
      {
        auto time = getSimulationTime();
        m->bFadingOutCountryMenu = true;
        m->countryMenuWidgetFadeOutAnimation->beginAnimation(m->countryMenuWidgetFadeInAnimation->getValue(time), 0.0f, time);
      }
    }

    auto trimmedChange = change;
    if (button == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
    {
      trimmedChange *= ((distance - m->paramEarthRadius) / (m->paramCameraMaxDistance - m->paramEarthRadius)) * m->paramCameraZoomSpeedFactor;

      latLong.set(
        osg::clampBetween<float>(latLong.x() - trimmedChange.y() * m->paramCameraScrollSpeed, -m->paramCameraMaxLatitude, m->paramCameraMaxLatitude),
        latLong.y() - trimmedChange.x() * m->paramCameraScrollSpeed);

      m->setFastCameraMotion();
      setCameraLatLong(latLong, getSimulationTime());
    }
    else if (button == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
    {
      auto clamp_to = std::max<float>(m->getViewAngleForDistance(distance) * 1.3f, atan(m->paramEarthRadius * 1.3 / distance));

      viewAngle.set(
        viewAngle.x() + trimmedChange.x() * m->paramCameraRotationSpeed,
        osg::clampBetween<float>(viewAngle.y() + trimmedChange.y() * m->paramCameraRotationSpeed, 0.0f, clamp_to));

      setCameraViewAngle(viewAngle, getSimulationTime());
    }
  }

  void GlobeInteractionState::onDragEndEvent(int button, const osg::Vec2f& origin, const osg::Vec2f& position)
  {
    if (button == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
    {
      m->bDraggingMidMouse = false;
      setCameraViewAngle(
        osg::Vec2f(
          0.0f,
          m->getViewAngleForDistance(getCameraDistance())),
        getSimulationTime());

      if (m->hoveredCountry != 0)
        m->countryOverlay->setHoveredCountryId(m->hoveredCountry);
    }
  }

  void GlobeInteractionState::onResizeEvent(float width, float height)
  {
    m->mainFrameWidget->setGeometry(0, 0, int(width), int(height));
  }

  osg::ref_ptr<osgGaming::Hud> GlobeInteractionState::injectHud(osgGaming::Injector&                 injector,
                                                                const osg::ref_ptr<osgGaming::View>& view)
  {
    return injector.inject<osgGaming::Hud>();
  }

  void GlobeInteractionState::setupUi()
  {
    auto resolution = getView(0)->getResolution();

    m->mainFrameWidget = new MainFrameWidget(*m->pInjector);
    m->mainFrameWidget->setGeometry(0, 0, int(resolution.x()), int(resolution.y()));

    QObject::connect(m->mainFrameWidget, &MainFrameWidget::clickedButtonDebug, [this]()
    {
      if (m->debugWindow == nullptr)
      {
        m->debugWindow = new DebugWindow(*m->pInjector);
      }
       
      if (!m->debugWindow->isVisible())
      {
        auto geo = m->debugWindow->geometry();
        m->debugWindow->setGeometry(20, 100, geo.width(), geo.height());
        m->debugWindow->show();
      }
    });

    m->countryMenuWidget->setCenterPosition(500, 500);

    getOverlayCompositor()->addVirtualOverlay(m->mainFrameWidget);
    getOverlayCompositor()->addVirtualOverlay(m->countryMenuWidget);

    m->countryMenuWidget->setVisible(false);
  }
}
