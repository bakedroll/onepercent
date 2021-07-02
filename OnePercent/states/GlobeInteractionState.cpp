#include "GlobeInteractionState.h"

#include "nodes/CountryOverlay.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaCountry.h"
#include "scripting/LuaCountriesTable.h"
#include "simulation/Simulation.h"
#include "simulation/ModelContainer.h"
#include "widgets/CountryMenuWidget.h"
#include "widgets/MainFrameWidget.h"
#include "widgets/DebugWindow.h"

#include <QMouseEvent>

#include <osgHelper/ioc/Injector.h>
#include <osgHelper/Helper.h>
#include <osgHelper/View.h>

#include <luaHelper/LuaStateManager.h>

#include <QtOsgBridge/Helper.h>
#include <QtOsgBridge/Macros.h>

namespace onep
{
  const auto CountryNone = 0;

  struct GlobeInteractionState::Impl
  {
    Impl(osgHelper::ioc::Injector& injector, GlobeInteractionState* b)
      : base(b),
        pInjector(&injector),
        configManager(injector.inject<LuaConfig>()),
        simulation(injector.inject<Simulation>()),
        countryOverlay(injector.inject<CountryOverlay>()),
        lua(injector.inject<luaHelper::LuaStateManager>()),
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
        bDraggingMidMouse(false),
        bFastAnimation(false),
        simData({0.0, 0.0})
    {
    }

    GlobeInteractionState* base;

    // only for debug window
    osgHelper::ioc::Injector* pInjector;

    osg::ref_ptr<LuaConfig>         configManager;
    osg::ref_ptr<Simulation>        simulation;
    osg::ref_ptr<CountryOverlay>    countryOverlay;
    osg::ref_ptr<osgHelper::View>   view;
    osg::ref_ptr<osgHelper::Camera> camera;

    osg::ref_ptr<luaHelper::LuaStateManager> lua;

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

    osgHelper::Observer<int>::Ptr selectedCountryObserver;

    CountryMenuWidget* countryMenuWidget;
    MainFrameWidget*   mainFrameWidget;
    DebugWindow*       debugWindow;

    osg::Vec2f lastDragPosition;

    osg::Vec2i mousePos;

    bool bDraggingMidMouse;
    bool bFastAnimation;

    SimulationData simData;

    float getViewAngleForDistance(float distance) const
    {
      float x = (paramCameraMaxDistance - distance) / (paramCameraMaxDistance - paramCameraMinDistance);
      return std::pow(x, 6.0f) * C_PI * 0.25f;
    }

    void setCameraDistanceAndAngle(float distance, float time = -1.0f)
    {
      if (!bDraggingMidMouse)
        base->setCameraViewAngle(osg::Vec2f(0.0f, getViewAngleForDistance(distance)), time);
      base->setCameraDistance(distance, time);
    }

    int pickCountryIdAt(const osg::Vec2i& pos)
    {
      osg::Vec3f point, direction, pickResult;

      camera->pickRay(float(pos.x()), float(pos.y()), point, direction);

      if (osgHelper::sphereLineIntersection(osg::Vec3f(0.0f, 0.0f, 0.0f), paramEarthRadius, point, direction, pickResult))
      {
        osg::Vec2f polar = osgHelper::getPolarFromCartesian(pickResult);
        return countryOverlay->getCountryId(polar);
      }

      return 0;
    }

    void setFastCameraMotion()
    {
      if (bFastAnimation)
        return;

      bFastAnimation = true;
      base->stopMotion(simData.time);
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
        base->setCameraMotionEase(osgHelper::AnimationEase::CIRCLE_OUT);
        bReady = true;
      }

      return bReady;
    }

    void updateCountryMenuWidgetPositionAndFade(int id)
    {
      osg::Vec2f latLong = countryOverlay->getCountryPresenter(id)->getCenterLatLong();

      osg::Vec3f position = osgHelper::getVec3FromEuler(latLong.x(), 0.0, latLong.y()) * paramEarthRadius;

      osg::Matrix win     = camera->getViewport()->computeWindowMatrix();
      osg::Matrix viewMat = camera->getViewMatrix();
      osg::Matrix projMat = camera->getProjectionMatrix();

      osg::Vec3 screen = position * viewMat * projMat * win;

      countryMenuWidget->setCenterPosition(static_cast<int>(screen.x()),
                                           static_cast<int>(view->getResolution().y() - screen.y()));

      countryMenuWidget->updateFade(simData.time);
    }
  };

  GlobeInteractionState::GlobeInteractionState(osgHelper::ioc::Injector& injector)
    : GlobeCameraState(injector)
    , m(new Impl(injector, this))
  {
  }

  GlobeInteractionState::~GlobeInteractionState() = default;

  void GlobeInteractionState::onInitialize(QPointer<QtOsgBridge::MainWindow> mainWindow, const SimulationData& data)
  {
    GlobeCameraState::onInitialize(mainWindow, data);

    m->simData = data;

    m->view   = mainWindow->getViewWidget()->getView();
    m->camera = m->view->getCamera(osgHelper::View::CameraType::Scene);

    m->paramEarthRadius = m->configManager->getNumber<float>("earth.radius");
    m->paramCameraMinDistance = m->configManager->getNumber<float>("camera.min_distance");
    m->paramCameraMaxDistance = m->configManager->getNumber<float>("camera.max_distance");
    m->paramCameraMaxLatitude = m->configManager->getNumber<float>("camera.max_latitude");
    m->paramCameraZoomSpeed = m->configManager->getNumber<float>("camera.zoom_speed");
    m->paramCameraZoomSpeedFactor = m->configManager->getNumber<float>("camera.zoom_speed_factor");
    m->paramCameraScrollSpeed = m->configManager->getNumber<float>("camera.scroll_speed");
    m->paramCameraRotationSpeed = m->configManager->getNumber<float>("camera.rotation_speed");

    const auto& presenters  = m->countryOverlay->getCountryPresenters();
    auto        projAngle   = static_cast<float>(m->camera->getProjectionAngle());
    auto        projRatio   = static_cast<float>(m->camera->getProjectionRatio());
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
      const auto isFadingOut = (m->countryMenuWidget->getCurrentFadeMode() == QtOsgBridge::FadeOverlay::FadeMode::Out);

      if ((id == 0) || (m->selectedCountry != id) && !isFadingOut)
      {
        m->countryMenuWidget->jumpTo(QtOsgBridge::FadeOverlay::FadeMode::Out);
      }

      if (id > 0)
      {
        auto time = m->simData.time;
        auto r = m->paramEarthRadius;

        auto countryPresenter = m->countryOverlay->getCountryPresenter(id);
        auto country          = m->modelContainer->getModel()->getCountriesTable()->getCountryById(id);

        assert_return(country);
        
        OSGH_QLOG_INFO(QString("Selected country (%1): %2").arg(country->getId()).arg(QString::fromLocal8Bit(country->getName().c_str())));

        float viewDistance = countryPresenter->getOptimalCameraDistance(
          float(m->camera->getProjectionAngle()),
          float(m->camera->getProjectionRatio()));

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
        OSGH_QLOG_DEBUG(QString("Min: %1 Max: %2 ViewDistance: %3 x: %9 Angle: %4 Distance: %5 %10 %11 Shift: %6 Lat: %7 cosa: %8")
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

        if ((m->selectedCountry != id) || isFadingOut)
        {
          m->countryMenuWidget->fadeTo(QtOsgBridge::FadeOverlay::FadeMode::In, time);
          m->countryMenuWidget->setCountry(country);
          m->updateCountryMenuWidgetPositionAndFade(id);
        }
      }

      m->selectedCountry = id;
    });

    setCameraMotionDuration(2.0);
    setCameraMotionEase(osgHelper::AnimationEase::SMOOTHER);

    m->setCameraDistanceAndAngle(m->paramCameraMaxDistance, static_cast<float>(m->simData.time));

    setupUi(mainWindow);

    connect(m->mainFrameWidget, &MainFrameWidget::toggledWidgetEnabled, [this](bool enabled)
    {
      m->isInteractionEnabled = !enabled;
      if (!m->isInteractionEnabled)
      {
        m->countryOverlay->setSelectedCountry(CountryNone);
      }
    });
  }

  void GlobeInteractionState::onUpdate(const SimulationData& data)
  {
    GlobeCameraState::onUpdate(data);

    m->simData = data;

    if (m->selectedCountry > 0)
    {
      m->updateCountryMenuWidgetPositionAndFade(m->selectedCountry);
    }

    const auto id = m->isInteractionEnabled ? m->pickCountryIdAt(m->mousePos) : CountryNone;
    if (id != m->hoveredCountry)
    {
      m->hoveredCountry = id;

      if (!m->bDraggingMidMouse)
        m->countryOverlay->setHoveredCountryId(id);
    }
  }

  void GlobeInteractionState::onExit()
  {
    QtOsgBridge::Helper::deleteWidget(m->mainFrameWidget);
    QtOsgBridge::Helper::deleteWidget(m->countryMenuWidget);
  }

  bool GlobeInteractionState::onKeyEvent(QKeyEvent* event)
  {
    switch (event->type())
    {
    case QMouseEvent::Type::KeyPress:
    {
      switch (event->key())
      {
      case Qt::Key::Key_Space:
      {
        if (m->simulation->isRunning())
        {
          m->simulation->stop();

          OSGH_LOG_INFO("Simulation stopped");
        }
        else
        {
          m->simulation->start();

          OSGH_LOG_INFO("Simulation started");
        }
        return true;
      }
      default:
        break;
      }

      break;
    }
    default:
      break;
    }

    return false;
  }

  bool GlobeInteractionState::onMouseEvent(QMouseEvent* event)
  {
    switch (event->type())
    {
    case QMouseEvent::Type::MouseButtonPress:
    {
      switch (event->button())
      {
      case Qt::MouseButton::LeftButton:
      {
        if (!m->ready() || !m->isInteractionEnabled)
        {
          return false;
        }

        int selected = m->pickCountryIdAt(m->mousePos);

        m->lua->safeExecute([this, selected]()
        { 
          m->countryOverlay->setSelectedCountry(selected);
        });

        return true;
      }
      default:
        break;
      }

      break;
    }
    case QMouseEvent::Type::MouseMove:
    {
      m->mousePos.set(event->pos().x(), m->view->getResolution().y() - event->pos().y());
      return true;
    }
    default:
      break;
    }

    return false;
  }

  bool GlobeInteractionState::onWheelEvent(QWheelEvent* event)
  {
    if (!m->ready() || !m->isInteractionEnabled)
    {
      return false;
    }

    float distance = getCameraDistance();

    if (event->angleDelta().y() > 0)
    {
      distance = distance * m->paramCameraZoomSpeed;
    }
    else if (event->angleDelta().y() < 0)
    {
      distance = distance * (1.0f / m->paramCameraZoomSpeed);
    }
    else
    {
      return false;
    }

    distance = osg::clampBetween<float>(distance, m->paramCameraMinDistance, m->paramCameraMaxDistance);

    m->setFastCameraMotion();
    m->setCameraDistanceAndAngle(distance, static_cast<float>(m->simData.time));

    return true;
  }

  void GlobeInteractionState::onDragBegin(Qt::MouseButton button, const osg::Vec2f& origin)
  {
    if (!m->isInteractionEnabled)
    {
      return;
    }

    m->lastDragPosition = origin;

    if (button == Qt::MouseButton::MiddleButton)
    {
      m->bDraggingMidMouse = true;
      if (m->hoveredCountry != 0)
        m->countryOverlay->setHoveredCountryId(0);
    }
  }

  void GlobeInteractionState::onDragMove(Qt::MouseButton button, const osg::Vec2f& origin, const osg::Vec2f& position,
                                         const osg::Vec2f& change)
  {
    if (!m->ready() || !m->isInteractionEnabled)
	  {
      return;
	  }

    auto latLong   = getCameraLatLong();
    auto distance  = getCameraDistance();
    auto viewAngle = getCameraViewAngle();

    if ((button != Qt::MouseButton::LeftButton) && (m->selectedCountry > 0))
    {
      m->countryMenuWidget->fadeTo(QtOsgBridge::FadeOverlay::FadeMode::Out, m->simData.time);
    }

    auto trimmedChange = change;
    trimmedChange.x() = -trimmedChange.x();

    if (button == Qt::MouseButton::RightButton)
    {
      trimmedChange *= ((distance - m->paramEarthRadius) / (m->paramCameraMaxDistance - m->paramEarthRadius)) * m->paramCameraZoomSpeedFactor;

      latLong.set(
        osg::clampBetween<float>(latLong.x() - trimmedChange.y() * m->paramCameraScrollSpeed, -m->paramCameraMaxLatitude, m->paramCameraMaxLatitude),
        latLong.y() - trimmedChange.x() * m->paramCameraScrollSpeed);

      m->setFastCameraMotion();
      setCameraLatLong(latLong, m->simData.time);
    }
    else if (button == Qt::MouseButton::MiddleButton)
    {
      auto clamp_to = std::max<float>(m->getViewAngleForDistance(distance) * 1.3f, atan(m->paramEarthRadius * 1.3 / distance));

      viewAngle.set(
        viewAngle.x() + trimmedChange.x() * m->paramCameraRotationSpeed,
        osg::clampBetween<float>(viewAngle.y() + trimmedChange.y() * m->paramCameraRotationSpeed, 0.0f, clamp_to));

      setCameraViewAngle(viewAngle, m->simData.time);
    }
  }

  void GlobeInteractionState::onDragEnd(Qt::MouseButton button, const osg::Vec2f& origin, const osg::Vec2f& position)
  {
    if (button == Qt::MouseButton::MiddleButton)
    {
      m->bDraggingMidMouse = false;
      setCameraViewAngle(
        osg::Vec2f(
          0.0f,
          m->getViewAngleForDistance(getCameraDistance())),
        m->simData.time);

      if (m->hoveredCountry != 0)
        m->countryOverlay->setHoveredCountryId(m->hoveredCountry);
    }
  }

  void GlobeInteractionState::onResizeEvent(QResizeEvent* event)
  {
    m->mainFrameWidget->setGeometry(0, 0, event->size().width(), event->size().height());
  }

  void GlobeInteractionState::setupUi(const QPointer<QtOsgBridge::MainWindow>& mainWindow)
  {
    auto resolution = m->view->getResolution();

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

    mainWindow->getViewWidget()->addOverlayWidget(m->mainFrameWidget);
    mainWindow->getViewWidget()->addVirtualOverlayWidget(m->countryMenuWidget);
  }
}
