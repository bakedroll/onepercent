#include "GlobeInteractionState.h"

#include "core/Globals.h"
#include "core/QConnectFunctor.h"
#include "nodes/GlobeOverviewWorld.h"
#include "nodes/GlobeModel.h"
#include "nodes/CountryOverlay.h"
#include "widgets/OverlayCompositor.h"
#include "widgets/VirtualOverlay.h"
#include "widgets/CountryMenuWidget.h"
#include "widgets/DebugWindow.h"

#include <osgGA/GUIEventAdapter>

#include <osgGaming/Helper.h>
#include <osgGaming/UIButton.h>
#include <simulation/CountryData.h>
#include <osgGaming/Hud.h>
#include <osgGaming/View.h>
#include <osgGaming/PropertiesManager.h>

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace onep
{
  class DebugButton : public osgGaming::UIButton
  {
  public:
    typedef enum _type
    {
      BRANCH,
      SKILL,
      OVERLAY
    } Type;

    DebugButton(Type type, int id, BranchType branch = BRANCH_UNDEFINED)
      : UIButton()
      , m_branch(branch)
      , m_id(id)
      , m_type(type)
    {

    }

    int getId()
    {
      return m_id;
    }

    Type getType()
    {
      return m_type;
    }

    BranchType getBranchType()
    {
      return m_branch;
    }

  private:
    BranchType m_branch;
    int m_id;
    Type m_type;

  };

  struct GlobeInteractionState::Impl
  {
    Impl(osgGaming::Injector& injector, GlobeInteractionState* b)
    : base(b)
    , propertiesManager(injector.inject<osgGaming::PropertiesManager>())
    , globeOverviewWorld(injector.inject<GlobeOverviewWorld>())
    , simulation(injector.inject<Simulation>())
    , countryOverlay(injector.inject<CountryOverlay>())
    , boundariesMesh(injector.inject<BoundariesMesh>())
    , bReady(false)
    , selectedCountry(0)
    , labelDays(nullptr)
    , countryMenuWidget(nullptr)
    , debugWindow(nullptr)
    , countryMenuWidgetFadeInAnimation(new osgGaming::Animation<float>(0.0f, 0.4f, osgGaming::AnimationEase::CIRCLE_IN))
    , countryMenuWidgetFadeOutAnimation(new osgGaming::Animation<float>(0.0f, 0.4f, osgGaming::AnimationEase::CIRCLE_OUT))
    , bFadingOutCountryMenu(false)
    {

    }

    GlobeInteractionState* base;

    osg::ref_ptr<osgGaming::PropertiesManager> propertiesManager;
    osg::ref_ptr<GlobeOverviewWorld> globeOverviewWorld;
    osg::ref_ptr<Simulation> simulation;
    osg::ref_ptr<CountryOverlay> countryOverlay;
    osg::ref_ptr<BoundariesMesh> boundariesMesh;

    float paramEarthRadius;
    float paramCameraMinDistance;
    float paramCameraMaxDistance;
    float paramCameraMaxLatitude;
    float paramCameraZoomSpeed;
    float paramCameraZoomSpeedFactor;
    float paramCameraScrollSpeed;
    float paramCameraRotationSpeed;

    bool bReady;

    int selectedCountry;

    osgGaming::Observer<int>::Ptr selectedCountryObserver;
    osgGaming::Observer<int>::Ptr dayObserver;

    QLabel* labelDays;

    CountryMenuWidget* countryMenuWidget;

    VirtualOverlay* mainOverlay;

    DebugWindow* debugWindow;

    osg::ref_ptr<osgGaming::Animation<float>> countryMenuWidgetFadeInAnimation;
    osg::ref_ptr<osgGaming::Animation<float>> countryMenuWidgetFadeOutAnimation;

    osg::Vec2i mousePos;

    bool bFadingOutCountryMenu;

    CountryMesh::Ptr pickCountryMeshAt(const osg::Vec2i& pos)
    {
      osg::Vec3f point, direction, pickResult;
      base->getWorld(base->getView(0))->getCameraManipulator()->getPickRay(float(pos.x()), float(pos.y()), point, direction);

      if (osgGaming::sphereLineIntersection(osg::Vec3f(0.0f, 0.0f, 0.0f), paramEarthRadius, point, direction, pickResult))
      {
        osg::Vec2f polar = osgGaming::getPolarFromCartesian(pickResult);
        osg::ref_ptr<CountryMesh> countryMesh = countryOverlay->getCountryMesh(polar);

        return countryMesh;
      }

      return nullptr;
    }

    bool ready()
    {
      if (!bReady && !base->isCameraInMotion())
      {
        base->setCameraMotionDuration(0.5);
        base->setCameraMotionEase(osgGaming::AnimationEase::CIRCLE_OUT);
        bReady = true;
      }

      return bReady;
    }

    void updateCountryMenuWidgetPosition(int id)
    {
      osg::ref_ptr<CountryData> data = countryOverlay->getCountryMesh(id)->getCountryData();
      osg::Vec2f latLong = data->getCenterLatLong();

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

    m->paramEarthRadius = m->propertiesManager->getValue<float>(Param_EarthRadiusName);
    m->paramCameraMinDistance = m->propertiesManager->getValue<float>(Param_CameraMinDistanceName);
    m->paramCameraMaxDistance = m->propertiesManager->getValue<float>(Param_CameraMaxDistanceName);
    m->paramCameraMaxLatitude = m->propertiesManager->getValue<float>(Param_CameraMaxLatitudeName);
    m->paramCameraZoomSpeed = m->propertiesManager->getValue<float>(Param_CameraZoomSpeedName);
    m->paramCameraZoomSpeedFactor = m->propertiesManager->getValue<float>(Param_CameraZoomSpeedFactorName);
    m->paramCameraScrollSpeed = m->propertiesManager->getValue<float>(Param_CameraScrollSpeedName);
    m->paramCameraRotationSpeed = m->propertiesManager->getValue<float>(Param_CameraRotationSpeedName);

    getHud(getView(0))->setFpsEnabled(true);

    m->dayObserver = m->simulation->getDayObs()->connect(osgGaming::Func<int>([this](int day)
    {
      m->labelDays->setText(QObject::tr("Day %1").arg(day));
    }));

    m->selectedCountryObserver = m->countryOverlay->getSelectedCountryIdObservable()->connect(osgGaming::Func<int>([this](int id)
    {
      if (id > 0)
      {
        CountryMesh::Ptr countryMesh = m->countryOverlay->getCountryMesh(id);

        printf("Selected country (%d): %s\n", countryMesh->getCountryData()->getId(), countryMesh->getCountryData()->getCountryName().c_str());

        setCameraLatLong(countryMesh->getCountryData()->getCenterLatLong(), getSimulationTime());
        setCameraDistance(std::max<float>(countryMesh->getCountryData()->getOptimalCameraDistance(
          float(getWorld(getView(0))->getCameraManipulator()->getProjectionAngle()),
          float(getWorld(getView(0))->getCameraManipulator()->getProjectionRatio())), m->paramCameraMinDistance), getSimulationTime());

        if (m->selectedCountry != id || m->bFadingOutCountryMenu)
        {
          m->bFadingOutCountryMenu = false;
          m->countryMenuWidget->setVisible(true);

          if (m->selectedCountry != id)
          {
            m->countryMenuWidgetFadeInAnimation->beginAnimation(0.0f, 0.8f, getSimulationTime());
            m->countryMenuWidget->setCountryMesh(countryMesh);
            m->updateCountryMenuWidgetPosition(id);
          }
          else
          {
            double time = getSimulationTime();
            m->countryMenuWidgetFadeInAnimation->beginAnimation(m->countryMenuWidgetFadeOutAnimation->getValue(time), 0.8f, time);
          }
        }
      }
      else
      {
        m->countryMenuWidget->setVisible(false);
      }

      m->selectedCountry = id;
    }));

    setCameraMotionDuration(2.0);
    setCameraMotionEase(osgGaming::AnimationEase::SMOOTHER);

    setCameraDistance(28.0f, getSimulationTime());
    setCameraViewAngle(osg::Vec2f(0.0f, 0.0f), getSimulationTime());

    setupUi();
  }

  osgGaming::AbstractGameState::StateEvent* GlobeInteractionState::update()
  {
    StateEvent* e = GlobeCameraState::update();

    if (m->selectedCountry > 0 && m->countryMenuWidget->isVisible())
      m->updateCountryMenuWidgetPosition(m->selectedCountry);

    CountryMesh::Ptr hovered = m->pickCountryMeshAt(m->mousePos);
    m->countryOverlay->setHoveredCountry(hovered);

    return e;
  }

  void GlobeInteractionState::onMousePressedEvent(int button, float x, float y)
  {
    if (!m->ready())
    {
      return;
    }

    if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
    {
      CountryMesh::Ptr countryMesh = m->pickCountryMeshAt(m->mousePos);

      int selected = countryMesh == nullptr ? 0 : int(countryMesh->getCountryData()->getId());
      m->countryOverlay->setSelectedCountry(selected);
    }
  }

  void GlobeInteractionState::onKeyPressedEvent(int key)
  {
    if (key == osgGA::GUIEventAdapter::KEY_Space)
    {
      if (m->simulation->running())
      {
        m->simulation->stop();
        printf("Simulation stopped\n");
      }
      else
      {
        m->simulation->start();
        printf("Simulation started\n");
      }
    }
  }

  void GlobeInteractionState::onMouseMoveEvent(float x, float y)
  {
    m->mousePos.set(x, y);
  }

  void GlobeInteractionState::onScrollEvent(osgGA::GUIEventAdapter::ScrollingMotion motion)
  {
    if (!m->ready())
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

    setCameraDistance(distance, getSimulationTime());
  }

  void GlobeInteractionState::onDragEvent(int button, osg::Vec2f origin, osg::Vec2f position, osg::Vec2f change)
  {
    if (!m->ready())
      return;

    osg::Vec2f latLong = getCameraLatLong();
    float distance = getCameraDistance();
    osg::Vec2f viewAngle = getCameraViewAngle();

    if (button != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
    {
      if (m->selectedCountry > 0 && !m->bFadingOutCountryMenu)
      {
        double time = getSimulationTime();
        m->bFadingOutCountryMenu = true;
        m->countryMenuWidgetFadeOutAnimation->beginAnimation(m->countryMenuWidgetFadeInAnimation->getValue(time), 0.0f, time);
      }
    }

    if (button == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
    {
      change *= ((distance - m->paramEarthRadius) / (m->paramCameraMaxDistance - m->paramEarthRadius)) * m->paramCameraZoomSpeedFactor;

      latLong.set(
        osg::clampBetween<float>(latLong.x() - change.y() * m->paramCameraScrollSpeed, -m->paramCameraMaxLatitude, m->paramCameraMaxLatitude),
        latLong.y() - change.x() * m->paramCameraScrollSpeed);

      setCameraLatLong(latLong, getSimulationTime());
    }
    else if (button == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
    {
      float clamp_to = atan(m->paramEarthRadius * 1.3 / distance);

      viewAngle.set(
        viewAngle.x() + change.x() * m->paramCameraRotationSpeed,
        osg::clampBetween<float>(viewAngle.y() + change.y() * m->paramCameraRotationSpeed, 0.0f, clamp_to));

      setCameraViewAngle(viewAngle, getSimulationTime());
    }
  }

  void GlobeInteractionState::onDragEndEvent(int button, osg::Vec2f origin, osg::Vec2f position)
  {
    if (button == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
    {
      setCameraViewAngle(osg::Vec2f(0.0f, 0.0f), getSimulationTime());
    }
  }

  void GlobeInteractionState::onResizeEvent(float width, float height)
  {
    m->mainOverlay->setGeometry(0, 0, int(width), int(height));
  }

  osg::ref_ptr<osgGaming::Hud> GlobeInteractionState::injectHud(osgGaming::Injector& injector, osg::ref_ptr<osgGaming::View> view)
  {
    return injector.inject<osgGaming::Hud>();
  }

  void GlobeInteractionState::setupUi()
  {
    osg::Vec2f resolution = getView(0)->getResolution();

    m->mainOverlay = new VirtualOverlay();
    m->mainOverlay->setContentsMargins(5, 5, 5, 5);
    m->mainOverlay->setGeometry(0, 0, int(resolution.x()), int(resolution.y()));

    m->labelDays = new QLabel(QString());
    m->labelDays->setObjectName("LabelDays");

    QPushButton* debugButton = new QPushButton(QObject::tr("Debug Window"));
    debugButton->setObjectName("DebugButton");
    debugButton->setMaximumWidth(180);

    QConnectFunctor::connect(debugButton, SIGNAL(clicked()), [this]()
    {
      if (m->debugWindow == nullptr)
        m->debugWindow = new DebugWindow(m->countryOverlay, m->boundariesMesh, m->simulation, m->mainOverlay);
       
      if (!m->debugWindow->isVisible())
      {
        QRect geo = m->debugWindow->geometry();
        m->debugWindow->setGeometry(20, 100, geo.width(), geo.height());
        m->debugWindow->show();
      }
    });

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addStretch(1);
    leftLayout->addWidget(debugButton);

    QWidget* leftWidget = new QWidget();
    leftWidget->setContentsMargins(0, 0, 0, 0);
    leftWidget->setObjectName("WidgetLeftPanel");
    leftWidget->setLayout(leftLayout);

    QHBoxLayout* centralLayout = new QHBoxLayout();
    centralLayout->addWidget(leftWidget);
    centralLayout->addStretch(1);

    QHBoxLayout* footerLayout = new QHBoxLayout();
    footerLayout->addWidget(m->labelDays);
    footerLayout->addStretch(1);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(centralLayout, 1);
    layout->addLayout(footerLayout);

    m->mainOverlay->setLayout(layout);

    m->countryMenuWidget = new CountryMenuWidget(m->simulation);
    m->countryMenuWidget->setCenterPosition(500, 500);

    getOverlayCompositor()->addVirtualOverlay(m->mainOverlay);
    getOverlayCompositor()->addVirtualOverlay(m->countryMenuWidget);

    m->countryMenuWidget->setVisible(false);
  }
}
