#include "GlobeInteractionState.h"

#include "core/Globals.h"
#include "nodes/GlobeOverviewWorld.h"
#include "nodes/GlobeModel.h"
#include "widgets/VirtualOverlay.h"

#include <osgGaming/Helper.h>
#include <osgGaming/TimerFactory.h>
#include <osgGaming/UIStackPanel.h>
#include <osgGaming/UIButton.h>
#include <simulation/CountryData.h>
#include <osgGaming/UIGrid.h>
#include <osgGaming/Hud.h>
#include <osgGaming/View.h>
#include <osgGaming/UIText.h>
#include <osgGaming/PropertiesManager.h>
#include <osgGaming/Timer.h>
#include <osgGaming/UIRadioGroup.h>

#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace osg;
using namespace std;
using namespace osgGA;
using namespace osgGaming;

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
    , bReady(false)
    , bStarted(false)
    , selectedCountry(0)
    , paramEarthRadius(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_EarthRadiusName))
    , paramCameraMinDistance(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraMinDistanceName))
    , paramCameraMaxDistance(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraMaxDistanceName))
    , paramCameraMaxLatitude(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraMaxLatitudeName))
    , paramCameraZoomSpeed(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraZoomSpeedName))
    , paramCameraZoomSpeedFactor(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraZoomSpeedFactorName))
    , paramCameraScrollSpeed(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraScrollSpeedName))
    , paramCameraRotationSpeed(osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_CameraRotationSpeedName))
    , labelClickCountry(nullptr)
    , labelDays(nullptr)
    , labelClickAgain(nullptr)
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

    osg::ref_ptr<osgGaming::UIText> textCountryInfo;

    bool bReady;
    bool bStarted;

    int selectedCountry;

    osg::ref_ptr<osgGaming::Timer> simulationTimer;

    osgGaming::UIRadioGroup::Ptr branchRadioGroup;

    osgGaming::Observer<int>::Ptr selectedCountryObserver;
    osgGaming::Observer<bool>::Ptr skillBranchActivatedObserver;
    osgGaming::Observer<osgGaming::UIButton::Ptr>::Ptr selectedButtonObserver;

    osgGaming::Observer<int>::Ptr selectedCountryIdObserver;

    QLabel* labelDays;
    QLabel* labelClickCountry;
    QLabel* labelClickAgain;

    bool ready()
    {
      if (!bReady && !base->isCameraInMotion())
      {
        base->setCameraMotionDuration(0.5);
        base->setCameraMotionEase(AnimationEase::CIRCLE_OUT);
        bReady = true;
      }

      return bReady;
    }

    void updateCountryInfoText()
    {
      if (base->getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryId() == 0)
      {
        textCountryInfo->setText("");

        return;
      }

      CountryData::Ptr country = base->getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryMesh()->getCountryData();

      std::string infoText = country->getCountryName() + "\n";
      country->getValues()->getContainer()->debugPrintToString(infoText);
      textCountryInfo->setText(infoText);
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

    getHud(getView(0))->loadMarkupFromXmlResource("./GameData/data/ui/ingamehud.xml");
    getHud(getView(0))->setFpsEnabled(true);

    setupUi();

    m->selectedCountryObserver = getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryIdObservable()->connect(osgGaming::Func<int>([this](int id)
    {
      m->updateCountryInfoText();

      if (id > 0)
      {
        CountryMesh::Ptr countryMesh = getGlobeOverviewWorld()->getGlobeModel()->getCountryMesh(id);

        printf("Selected country (%d): %s\n", countryMesh->getCountryData()->getId(), countryMesh->getCountryData()->getCountryName().c_str());

        setCameraLatLong(countryMesh->getCountryData()->getCenterLatLong(), getSimulationTime());
        setCameraDistance(max(countryMesh->getCountryData()->getOptimalCameraDistance(
          float(getWorld(getView(0))->getCameraManipulator()->getProjectionAngle()),
          float(getWorld(getView(0))->getCameraManipulator()->getProjectionRatio())), m->paramCameraMinDistance), getSimulationTime());
      }
    }));

    setCameraMotionDuration(2.0);
    setCameraMotionEase(AnimationEase::SMOOTHER);

    setCameraDistance(28.0f, getSimulationTime());
    setCameraViewAngle(Vec2f(0.0f, 0.0f), getSimulationTime());
  }

  VirtualOverlay* GlobeInteractionState::createVirtualOverlay()
  {
    m->labelClickCountry = new QLabel(QObject::tr("Please select a country."));
	m->labelClickCountry->setStyleSheet("font-size: 18pt; color: #fff;");

    m->labelClickAgain = new QLabel(QObject::tr("Click again to confirm your selection."));
    m->labelClickAgain->setStyleSheet("font-size: 12pt; color: #fff;");
    m->labelClickAgain->setVisible(false);

    m->labelDays = new QLabel(QString());
    m->labelDays->setStyleSheet("font-size: 18pt; color: #fff;");
    m->labelDays->setVisible(false);

    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->addWidget(m->labelClickCountry);
    headerLayout->addWidget(m->labelClickAgain);

    QRadioButton* radioNoOverlay = new QRadioButton(QObject::tr("No Overlay"));
    radioNoOverlay->setChecked(true);

    QHBoxLayout* leftLayout = new QHBoxLayout();
    leftLayout->addWidget(radioNoOverlay);

    QWidget* leftWidget = new QWidget();
    leftWidget->setContentsMargins(0, 0, 0, 0);
    leftWidget->setFixedWidth(200);
    leftWidget->setLayout(leftLayout);

    QHBoxLayout* centralLayout = new QHBoxLayout();
    centralLayout->addWidget(leftWidget);
    centralLayout->addStretch(1);

    QHBoxLayout* footerLayout = new QHBoxLayout();
    footerLayout->addWidget(m->labelDays);
    footerLayout->addStretch(1);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(headerLayout);
    layout->addLayout(centralLayout, 1);
    layout->addLayout(footerLayout);

    VirtualOverlay* overlay = new VirtualOverlay();
    overlay->setContentsMargins(5, 5, 5, 5);
    overlay->setLayout(layout);

    return overlay;
  }

  void GlobeInteractionState::setupUi()
  {
    GlobeModel::Ptr globeModel = getGlobeOverviewWorld()->getGlobeModel();
    Simulation::Ptr simulation = getGlobeOverviewWorld()->getSimulation();

    m->textCountryInfo = static_cast<UIText*>(getHud(getView(0))->getUIElementByName("text_countryInfo").get());

    UIGrid* gridBranches = static_cast<UIGrid*>(getHud(getView(0))->getUIElementByName("grid_branchbuttons").get());
    UIStackPanel* stackPanelSkills = static_cast<UIStackPanel*>(getHud(getView(0))->getUIElementByName("panel_skillbuttons").get());

    // Branches
    gridBranches->getColumns()->setNumCells(2);
    gridBranches->getRows()->setNumCells(NUM_SKILLBRANCHES);

    stackPanelSkills->getCells()->setNumCells(simulation->getNumSkills() + NUM_SKILLBRANCHES + 1);
    stackPanelSkills->getCells()->setSizePolicy(simulation->getNumSkills() + NUM_SKILLBRANCHES, UICells::AUTO);
    int stackPanelSkillsPos = 0;

    m->branchRadioGroup = new UIRadioGroup();

    for (int i = 0; i < NUM_SKILLBRANCHES; i++)
    {
      osg::ref_ptr<UIButton> buttonEnabled = new DebugButton(DebugButton::BRANCH, i);
      buttonEnabled->setText(branch_getStringFromType(i));
      buttonEnabled->setCheckable(true);

      osg::ref_ptr<UIButton> buttonOverlay = new DebugButton(DebugButton::OVERLAY, i);
      buttonOverlay->setText("Overlay");

      m->selectedCountryIdObserver = globeModel->getSelectedCountryIdObservable()->connectAndNotify(osgGaming::Func<int>([globeModel, buttonEnabled, buttonOverlay, i](int selected)
      {
        if (selected > 0)
        {
          buttonEnabled->setChecked(globeModel->getCountryMesh(selected)->getCountryData()->getSkillBranchActivated(i));
        }
        else
        {
          buttonEnabled->setChecked(false);
        }

        buttonEnabled->setEnabled(selected > 0);
      }));

      // Countries
      CountryMesh::Map& countryMeshs = globeModel->getCountryMeshs();
      for (CountryMesh::Map::iterator it = countryMeshs.begin(); it != countryMeshs.end(); ++it)
      {
        m->skillBranchActivatedObserver = it->second->getCountryData()->getSkillBranchActivatedObservable(i)->connect(osgGaming::Func<bool>([globeModel, buttonEnabled, it](bool activated)
        {
          if (it->first == globeModel->getSelectedCountryId())
            buttonEnabled->setChecked(activated);
        }));
      }

      getHud(getView(0))->registerUserInteractionModel(buttonEnabled.get());
      getHud(getView(0))->registerUserInteractionModel(buttonOverlay.get());

      m->branchRadioGroup->addButton(buttonOverlay);

      gridBranches->addChild(buttonEnabled, osgGaming::ColRow(0, i));
      gridBranches->addChild(buttonOverlay, osgGaming::ColRow(1, i));

      // Skills
      int nskills = simulation->getSkillBranch(BranchType(i))->getNumSkills();
      UIText::Ptr text = new UIText();

      if (stackPanelSkillsPos > 0)
        text->setMargin(Vec4f(0.0f, 8.0f, 0.0f, 0.0f));
      text->setFontSize(13);
      text->setText(branch_getStringFromType(i));

      stackPanelSkills->addChild(text, stackPanelSkillsPos++);

      for (int j = 0; j < nskills; j++)
      {
        Skill::Ptr skill = simulation->getSkillBranch(BranchType(i))->getSkill(j);

        osg::ref_ptr<UIButton> button = new DebugButton(DebugButton::SKILL, j, BranchType(i));
        button->setText(skill->getName());
        button->setCheckable(true);

        getHud(getView(0))->registerUserInteractionModel(button.get());
        stackPanelSkills->addChild(button, stackPanelSkillsPos++);
      }
    }

    m->selectedButtonObserver = m->branchRadioGroup->getSelectedButtonObservable()->connect(osgGaming::Func<UIButton::Ptr>([globeModel](UIButton::Ptr button)
    {
      if (!button)
      {
        globeModel->clearHighlightedCountries();
        return;
      }

      DebugButton* dbutton = dynamic_cast<DebugButton*>(button.get());
      if (!dbutton || dbutton->getType() != DebugButton::OVERLAY)
        return;

      globeModel->setHighlightedSkillBranch(BranchType(dbutton->getId()));
    }));
  }

  void GlobeInteractionState::onMousePressedEvent(int button, float x, float y)
  {
    if (!m->ready())
    {
      return;
    }

    if (button == GUIEventAdapter::LEFT_MOUSE_BUTTON)
    {
      Vec3f point, direction;
      getWorld(getView(0))->getCameraManipulator()->getPickRay(x, y, point, direction);

      Vec3f pickResult;
      if (sphereLineIntersection(Vec3f(0.0f, 0.0f, 0.0f), m->paramEarthRadius, point, direction, pickResult))
      {
        Vec2f polar = getPolarFromCartesian(pickResult);
        ref_ptr<CountryMesh> countryMesh = getGlobeOverviewWorld()->getGlobeModel()->getCountryMesh(polar);

        int selected = countryMesh == nullptr ? 0 : int(countryMesh->getCountryData()->getId());

        getGlobeOverviewWorld()->getGlobeModel()->setSelectedCountry(selected);
        m->branchRadioGroup->reset();

        if (!m->bStarted)
        {
          if (selected == 0)
          {
            m->labelClickAgain->setVisible(false);
          }
          else
          {
            if (m->selectedCountry == selected)
            {
              startSimulation();
            }
            else
            {
              m->labelClickAgain->setVisible(true);
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
      if (key == GUIEventAdapter::KEY_Space)
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

  void GlobeInteractionState::onScrollEvent(GUIEventAdapter::ScrollingMotion motion)
  {
    if (!m->ready())
    {
      return;
    }

    float distance = getCameraDistance();

    if (motion == GUIEventAdapter::SCROLL_UP)
    {
      distance = distance * m->paramCameraZoomSpeed;
    }
    else if (motion == GUIEventAdapter::SCROLL_DOWN)
    {
      distance = distance * (1.0f / m->paramCameraZoomSpeed);
    }
    else
    {
      return;
    }

    distance = clampBetween(distance, m->paramCameraMinDistance, m->paramCameraMaxDistance);

    setCameraDistance(distance, getSimulationTime());
  }

  void GlobeInteractionState::onDragEvent(int button, Vec2f origin, Vec2f position, osg::Vec2f change)
  {
    if (!m->ready())
    {
      return;
    }

    Vec2f latLong = getCameraLatLong();
    float distance = getCameraDistance();
    Vec2f viewAngle = getCameraViewAngle();

    if (button == GUIEventAdapter::RIGHT_MOUSE_BUTTON)
    {
      change *= ((distance - m->paramEarthRadius) / (m->paramCameraMaxDistance - m->paramEarthRadius)) * m->paramCameraZoomSpeedFactor;

      latLong.set(
        clampBetween(latLong.x() - change.y() * m->paramCameraScrollSpeed, -m->paramCameraMaxLatitude, m->paramCameraMaxLatitude),
        latLong.y() - change.x() * m->paramCameraScrollSpeed);

      setCameraLatLong(latLong, getSimulationTime());
    }
    else if (button == GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
    {
      float clamp_to = atan(m->paramEarthRadius * 1.3 / distance);

      viewAngle.set(
        viewAngle.x() + change.x() * m->paramCameraRotationSpeed,
        clampBetween(viewAngle.y() + change.y() * m->paramCameraRotationSpeed, 0.0f, clamp_to));

      setCameraViewAngle(viewAngle, getSimulationTime());
    }
  }

  void GlobeInteractionState::onDragEndEvent(int button, osg::Vec2f origin, osg::Vec2f position)
  {
    if (button == GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
    {
      setCameraViewAngle(Vec2f(0.0f, 0.0f), getSimulationTime());
    }
  }

  void GlobeInteractionState::onUIClickedEvent(ref_ptr<UIElement> uiElement)
  {
    DebugButton* dbutton = dynamic_cast<DebugButton*>(uiElement.get());
    if (!dbutton)
      return;

    int selectedCountryId = getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryId();

    if (dbutton->getType() == DebugButton::BRANCH && selectedCountryId > 0)
    {
      ref_ptr<CountryData> selectedCountry = getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryMesh()->getCountryData();
      selectedCountry->setSkillBranchActivated(dbutton->getId(), dbutton->isChecked());
    }
    else if (dbutton->getType() == DebugButton::SKILL)
    {
      getGlobeOverviewWorld()->getSimulation()->getSkillBranch(dbutton->getBranchType())->getSkill(dbutton->getId())->setActivated(dbutton->isChecked());
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
    m->labelClickCountry->setVisible(false);
    m->labelClickAgain->setVisible(false);
    m->labelDays->setVisible(true);

    getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryMesh()->getCountryData()->setSkillBranchActivated(BRANCH_BANKS, true);

    m->simulationTimer = TimerFactory::getInstance()->create<GlobeInteractionState>(&GlobeInteractionState::dayTimerElapsed, this, 1.0, false);
    m->simulationTimer->start();

    m->bStarted = true;
  }

  ref_ptr<Hud> GlobeInteractionState::overrideHud(osg::ref_ptr<osgGaming::View> view)
  {
    return new Hud();
  }

}
