#include "GlobeInteractionState.h"

#include "core/Globals.h"
#include "core/QConnectFunctor.h"
#include "nodes/GlobeOverviewWorld.h"
#include "nodes/GlobeModel.h"
#include "widgets/VirtualOverlay.h"
#include "widgets/CountryMenuWidget.h"

#include <osgGA/GUIEventAdapter>

#include <osgGaming/Helper.h>
#include <osgGaming/TimerFactory.h>
#include <osgGaming/UIButton.h>
#include <simulation/CountryData.h>
#include <osgGaming/Hud.h>
#include <osgGaming/View.h>
#include <osgGaming/PropertiesManager.h>
#include <osgGaming/Timer.h>

#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QButtonGroup>

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
    Impl(GlobeInteractionState* b)
    : base(b)
    , paramEarthRadius(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_EarthRadiusName))
    , paramCameraMinDistance(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraMinDistanceName))
    , paramCameraMaxDistance(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraMaxDistanceName))
    , paramCameraMaxLatitude(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraMaxLatitudeName))
    , paramCameraZoomSpeed(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraZoomSpeedName))
    , paramCameraZoomSpeedFactor(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraZoomSpeedFactorName))
    , paramCameraScrollSpeed(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraScrollSpeedName))
    , paramCameraRotationSpeed(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraRotationSpeedName))
    , bReady(false)
    , bStarted(false)
    , selectedCountry(0)
    , labelDays(nullptr)
    , labelClickCountry(nullptr)
    , labelClickAgain(nullptr)
    , labelStats(nullptr)
    , radioNoOverlay(nullptr)
    , countryMenuWidget(nullptr)
    {

    }

    GlobeInteractionState* base;

    float paramEarthRadius;
    float paramCameraMinDistance;
    float paramCameraMaxDistance;
    float paramCameraMaxLatitude;
    float paramCameraZoomSpeed;
    float paramCameraZoomSpeedFactor;
    float paramCameraScrollSpeed;
    float paramCameraRotationSpeed;

    bool bReady;
    bool bStarted;

    int selectedCountry;

    osg::ref_ptr<osgGaming::Timer> simulationTimer;
    osgGaming::Observer<int>::Ptr selectedCountryObserver;

    std::vector<osgGaming::Observer<bool>::Ptr> skillBranchActivatedObservers;
    std::vector<osgGaming::Observer<int>::Ptr> selectedCountryIdObservers;

    QLabel* labelDays;
    QLabel* labelClickCountry;
    QLabel* labelClickAgain;
    QLabel* labelStats;
    QRadioButton* radioNoOverlay;

    CountryMenuWidget* countryMenuWidget;

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

    void updateCountryInfoText()
    {
      if (base->getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryId() == 0)
      {
        labelStats->setText("");
        return;
      }

      CountryData::Ptr country = base->getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryMesh()->getCountryData();

      std::string infoText = country->getCountryName() + "\n";
      country->getValues()->getContainer()->debugPrintToString(infoText);
      labelStats->setText(QString::fromStdString(infoText));
    }

    void updateCountryMenuWidgetPosition(int id)
    {
      osg::ref_ptr<CountryData> data = base->getGlobeOverviewWorld()->getGlobeModel()->getCountryMesh(id)->getCountryData();
      osg::Vec2f latLong = data->getCenterLatLong();

      osg::Vec3f position = osgGaming::getVec3FromEuler(latLong.x(), 0.0, latLong.y()) * paramEarthRadius;

      osg::ref_ptr<osg::Camera> cam = base->getView(0)->getSceneCamera();
      osg::Matrix win = cam->getViewport()->computeWindowMatrix();
      osg::Matrix view = base->getGlobeOverviewWorld()->getCameraManipulator()->getViewMatrix(); //cam->getViewMatrix();
      osg::Matrix proj = cam->getProjectionMatrix();

      osg::Vec3 screen = position * view * proj * win;

      countryMenuWidget->setCenterPosition(int(screen.x()), int(base->getView(0)->getResolution().y() - screen.y()));
    }
  };

  GlobeInteractionState::GlobeInteractionState()
    : GlobeCameraState()
    , m(new Impl(this))
  {
  }

  GlobeInteractionState::~GlobeInteractionState()
  {

  }

  void GlobeInteractionState::initialize()
  {
    GlobeCameraState::initialize();

    getHud(getView(0))->setFpsEnabled(true);

    m->selectedCountryObserver = getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryIdObservable()->connect(osgGaming::Func<int>([this](int id)
    {
      m->updateCountryInfoText();

      if (id > 0)
      {
        CountryMesh::Ptr countryMesh = getGlobeOverviewWorld()->getGlobeModel()->getCountryMesh(id);

        printf("Selected country (%d): %s\n", countryMesh->getCountryData()->getId(), countryMesh->getCountryData()->getCountryName().c_str());

        setCameraLatLong(countryMesh->getCountryData()->getCenterLatLong(), getSimulationTime());
        setCameraDistance(std::max<float>(countryMesh->getCountryData()->getOptimalCameraDistance(
          float(getWorld(getView(0))->getCameraManipulator()->getProjectionAngle()),
          float(getWorld(getView(0))->getCameraManipulator()->getProjectionRatio())), m->paramCameraMinDistance), getSimulationTime());

        m->countryMenuWidget->setVisible(true);
        m->updateCountryMenuWidgetPosition(id);
      }
      else
      {
        m->countryMenuWidget->setVisible(false);
      }
    }));

    setCameraMotionDuration(2.0);
    setCameraMotionEase(osgGaming::AnimationEase::SMOOTHER);

    setCameraDistance(28.0f, getSimulationTime());
    setCameraViewAngle(osg::Vec2f(0.0f, 0.0f), getSimulationTime());
  }

  VirtualOverlay* GlobeInteractionState::createVirtualOverlay()
  {
    GlobeModel::Ptr globeModel = getGlobeOverviewWorld()->getGlobeModel();
    Simulation::Ptr simulation = getGlobeOverviewWorld()->getSimulation();

    VirtualOverlay* overlay = new VirtualOverlay();
    overlay->setContentsMargins(5, 5, 5, 5);

    m->labelClickCountry = new QLabel(QObject::tr("Please select a country."));
    m->labelClickCountry->setObjectName("LabelSelectCountry");

    m->labelClickAgain = new QLabel(QString());
    m->labelClickAgain->setObjectName("LabelClickAgain");

    m->labelDays = new QLabel(QString());
    m->labelDays->setObjectName("LabelDays");
    m->labelDays->setVisible(false);

    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->addWidget(m->labelClickCountry);
    headerLayout->addWidget(m->labelClickAgain);

    QGridLayout* branchesLayout = new QGridLayout();

    QButtonGroup* radioGroup = new QButtonGroup(overlay);

    m->radioNoOverlay = new QRadioButton(QObject::tr("No Overlay"));
    m->radioNoOverlay->setChecked(true);
    radioGroup->addButton(m->radioNoOverlay);

    osgGaming::QConnectBoolFunctor::connect(m->radioNoOverlay, SIGNAL(clicked(bool)), [=](bool checked)
    {
      if (checked)
        globeModel->clearHighlightedCountries();
    });

    branchesLayout->addWidget(m->radioNoOverlay, 0, 1);
    for (int i = 0; i < NUM_SKILLBRANCHES; i++)
    {
      QCheckBox* checkBox = new QCheckBox(QString::fromStdString(branch_getStringFromType(i)));
      QRadioButton* radioButton = new QRadioButton(QObject::tr("Overlay"));
      radioGroup->addButton(radioButton);

      branchesLayout->addWidget(checkBox, 1 + i, 0);
      branchesLayout->addWidget(radioButton, 1 + i, 1);

      osgGaming::QConnectBoolFunctor::connect(checkBox, SIGNAL(clicked(bool)), [=](bool checked)
      {
        osg::ref_ptr<CountryData> selectedCountry = globeModel->getSelectedCountryMesh()->getCountryData();
        if (!selectedCountry.valid())
          return;

        selectedCountry->setSkillBranchActivated(i, checked);
      });

      osgGaming::QConnectBoolFunctor::connect(radioButton, SIGNAL(clicked(bool)), [=](bool checked)
      {
        globeModel->setHighlightedSkillBranch(BranchType(i));
      });

      m->selectedCountryIdObservers.push_back(globeModel->getSelectedCountryIdObservable()->connectAndNotify(osgGaming::Func<int>([=](int selected)
      {
        if (selected > 0)
        {
          checkBox->setChecked(globeModel->getCountryMesh(selected)->getCountryData()->getSkillBranchActivated(i));
        }
        else
        {
          checkBox->setChecked(false);
        }

        checkBox->setEnabled(selected > 0);
      })));

      CountryMesh::Map& countryMeshs = globeModel->getCountryMeshs();
      for (CountryMesh::Map::iterator it = countryMeshs.begin(); it != countryMeshs.end(); ++it)
      {
        m->skillBranchActivatedObservers.push_back(it->second->getCountryData()->getSkillBranchActivatedObservable(i)->connect(osgGaming::Func<bool>([=](bool activated)
        {
          if (it->first == globeModel->getSelectedCountryId())
            checkBox->setChecked(activated);
        })));
      }
    }

    m->labelStats = new QLabel(QString());

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addLayout(branchesLayout);
    leftLayout->addWidget(m->labelStats);
    leftLayout->addStretch(1);

    QWidget* leftWidget = new QWidget();
    leftWidget->setContentsMargins(0, 0, 0, 0);
    leftWidget->setObjectName("WidgetLeftPanel");
    leftWidget->setLayout(leftLayout);

    QVBoxLayout* rightLayout = new QVBoxLayout();

    // Skills
    for (int i = 0; i < NUM_SKILLBRANCHES; i++)
    {
      // Skills
      int nskills = simulation->getSkillBranch(BranchType(i))->getNumSkills();

      QLabel* label = new QLabel(QString::fromStdString(branch_getStringFromType(i)));
      rightLayout->addWidget(label);

      for (int j = 0; j < nskills; j++)
      {
        Skill::Ptr skill = simulation->getSkillBranch(BranchType(i))->getSkill(j);

        QCheckBox* checkBox = new QCheckBox(QString::fromStdString(skill->getName()));
        rightLayout->addWidget(checkBox);

        osgGaming::QConnectBoolFunctor::connect(checkBox, SIGNAL(clicked(bool)), [=](bool checked)
        {
          simulation->getSkillBranch(BranchType(i))->getSkill(j)->setActivated(checked);
        });
      }
    }

    rightLayout->addStretch(1);

    QWidget* rightWidget = new QWidget();
    rightWidget->setContentsMargins(0, 0, 0, 0);
    rightWidget->setObjectName("WidgetRightPanel");
    rightWidget->setLayout(rightLayout);

    QHBoxLayout* centralLayout = new QHBoxLayout();
    centralLayout->addWidget(leftWidget);
    centralLayout->addStretch(1);
    centralLayout->addWidget(rightWidget);

    QHBoxLayout* footerLayout = new QHBoxLayout();
    footerLayout->addWidget(m->labelDays);
    footerLayout->addStretch(1);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(headerLayout);
    layout->addLayout(centralLayout, 1);
    layout->addLayout(footerLayout);

    overlay->setLayout(layout);

	  m->countryMenuWidget = new CountryMenuWidget();
    m->countryMenuWidget->setParent(overlay);
    m->countryMenuWidget->setCenterPosition(500, 500);
    m->countryMenuWidget->setVisible(false);

    return overlay;
  }

  osgGaming::AbstractGameState::StateEvent* GlobeInteractionState::update()
  {
    StateEvent* e = GlobeCameraState::update();

    if (m->selectedCountry > 0)
      m->updateCountryMenuWidgetPosition(m->selectedCountry);

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
      osg::Vec3f point, direction;
      getWorld(getView(0))->getCameraManipulator()->getPickRay(x, y, point, direction);

      osg::Vec3f pickResult;
      if (osgGaming::sphereLineIntersection(osg::Vec3f(0.0f, 0.0f, 0.0f), m->paramEarthRadius, point, direction, pickResult))
      {
        osg::Vec2f polar = osgGaming::getPolarFromCartesian(pickResult);
        osg::ref_ptr<CountryMesh> countryMesh = getGlobeOverviewWorld()->getGlobeModel()->getCountryMesh(polar);

        int selected = countryMesh == nullptr ? 0 : int(countryMesh->getCountryData()->getId());

        getGlobeOverviewWorld()->getGlobeModel()->setSelectedCountry(selected);
        m->radioNoOverlay->setChecked(true);

        if (!m->bStarted)
        {
          if (selected == 0)
          {
            m->labelClickAgain->setText(QString());
          }
          else
          {
            if (m->selectedCountry == selected)
            {
              startSimulation();
            }
            else
            {
              m->labelClickAgain->setText(QObject::tr("Click again to confirm your selection."));
            }
          }
        }

        m->selectedCountry = selected;
      }
    }
  }

  void GlobeInteractionState::onKeyPressedEvent(int key)
  {
    /*if (!ready())
    {
    return;
    }*/

    if (m->bStarted)
    {
      if (key == osgGA::GUIEventAdapter::KEY_Space)
      {
        if (m->simulationTimer->running())
        {
          m->simulationTimer->stop();

          printf("Simulation stopped\n");
        }
        else
        {
          m->simulationTimer->start();

          printf("Simulation started\n");
        }
      }
      /*else if (key == GUIEventAdapter::KEY_P)
      {
      getGlobeOverviewWorld()->getSimulation()->printStats();
      }*/
    }
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
    {
      return;
    }

    osg::Vec2f latLong = getCameraLatLong();
    float distance = getCameraDistance();
    osg::Vec2f viewAngle = getCameraViewAngle();

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

  void GlobeInteractionState::dayTimerElapsed()
  {
    getGlobeOverviewWorld()->getSimulation()->step();

    std::string dayText = osgGaming::PropertiesManager::getInstance()->getValue<std::string>(Param_LocalizationInfoTextDay);
    m->labelDays->setText(QObject::tr("Day %1").arg(getGlobeOverviewWorld()->getSimulation()->getDay()));

    m->updateCountryInfoText();
  }

  void GlobeInteractionState::startSimulation()
  {
    m->labelClickCountry->setText(QString());
    m->labelClickAgain->setText(QString());
    m->labelDays->setVisible(true);

    getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryMesh()->getCountryData()->setSkillBranchActivated(BRANCH_BANKS, true);

    m->simulationTimer = osgGaming::TimerFactory::getInstance()->create<GlobeInteractionState>(&GlobeInteractionState::dayTimerElapsed, this, 1.0, false);
    m->simulationTimer->start();

    m->bStarted = true;
  }

  osg::ref_ptr<osgGaming::Hud> GlobeInteractionState::overrideHud(osg::ref_ptr<osgGaming::View> view)
  {
    return new osgGaming::Hud();
  }

}
