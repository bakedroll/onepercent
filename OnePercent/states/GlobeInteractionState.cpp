#include "GlobeInteractionState.h"

#include "nodes/GlobeModel.h"

#include <osgGaming/Helper.h>
#include <osgGaming/TimerFactory.h>
#include <osgGaming/UIStackPanel.h>
#include <osgGaming/UIButton.h>
#include <simulation/CountryData.h>
#include <osgGaming/UIGrid.h>

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

    DebugButton(Type type, int id)
      : UIButton()
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

  private:
    int m_id;
    Type m_type;

  };

  GlobeInteractionState::GlobeInteractionState()
    : GlobeCameraState(),
    _ready(false),
    _started(false),
    _selectedCountry(0)
  {
  }

  void GlobeInteractionState::initialize()
  {
    GlobeCameraState::initialize();

    getHud()->loadMarkupFromXmlResource("./GameData/data/ui/ingamehud.xml");
    getHud()->setFpsEnabled(true);

    setupUi();

    getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryIdObservable()->addNotifyFunc([this](int id)
    {
      updateCountryInfoText();

      if (id > 0)
      {
        CountryMesh::Ptr countryMesh = getGlobeOverviewWorld()->getGlobeModel()->getCountryMesh(id);

        printf("Selected country (%d): %s\n", countryMesh->getCountryData()->getId(), countryMesh->getCountryData()->getCountryName().c_str());

        setCameraLatLong(countryMesh->getCountryData()->getCenterLatLong(), getSimulationTime());
        setCameraDistance(max(countryMesh->getCountryData()->getOptimalCameraDistance(
          float(getWorld()->getCameraManipulator()->getProjectionAngle()),
          float(getWorld()->getCameraManipulator()->getProjectionRatio())), ~_paramCameraMinDistance), getSimulationTime());
      }
    });

    setCameraMotionDuration(2.0);
    setCameraMotionEase(AnimationEase::SMOOTHER);

    setCameraDistance(28.0f, getSimulationTime());
    setCameraViewAngle(Vec2f(0.0f, 0.0f), getSimulationTime());
  }

  void GlobeInteractionState::setupUi()
  {
    _textPleaseSelect = static_cast<UIText*>(getHud()->getUIElementByName("text_selectCountry").get());
    _textConfirm = static_cast<UIText*>(getHud()->getUIElementByName("text_selectCountryAgain").get());
    _textProgress = static_cast<UIText*>(getHud()->getUIElementByName("text_dayProgress").get());
    _textCountryInfo = static_cast<UIText*>(getHud()->getUIElementByName("text_countryInfo").get());

    UIGrid* gridBranches = static_cast<UIGrid*>(getHud()->getUIElementByName("grid_branchbuttons").get());
    UIStackPanel* stackPanelSkills = static_cast<UIStackPanel*>(getHud()->getUIElementByName("panel_skillbuttons").get());

    // Branches
    gridBranches->getColumns()->setNumCells(2);
    gridBranches->getRows()->setNumCells(NUM_SKILLBRANCHES);

    m_branchRadioGroup = new UIRadioGroup();

    for (int i = 0; i < NUM_SKILLBRANCHES; i++)
    {
      osg::ref_ptr<UIButton> buttonEnabled = new DebugButton(DebugButton::BRANCH, i);
      buttonEnabled->setText(branch_getStringFromType(i));
      buttonEnabled->setCheckable(true);

      osg::ref_ptr<UIButton> buttonOverlay = new DebugButton(DebugButton::OVERLAY, i);
      buttonOverlay->setText("Overlay");

      getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryIdObservable()->addFuncAndNotify([this, buttonEnabled, buttonOverlay, i](int selected)
      {
        if (selected > 0)
        {
          buttonEnabled->setChecked(getGlobeOverviewWorld()->getGlobeModel()->getCountryMesh(selected)->getCountryData()->getSkillBranchActivated(i));
        }
        else
        {
          buttonEnabled->setChecked(false);
        }

        buttonEnabled->setEnabled(selected > 0);
      });

      // Countries
      CountryMesh::Map& countryMeshs = getGlobeOverviewWorld()->getGlobeModel()->getCountryMeshs();
      for (CountryMesh::Map::iterator it = countryMeshs.begin(); it != countryMeshs.end(); ++it)
      {
        it->second->getCountryData()->getSkillBranchActivatedObservable(i)->addNotifyFunc([this, buttonEnabled, it](bool activated)
        {
          if (it->first == getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryId())
            buttonEnabled->setChecked(activated);
        });
      }

      getHud()->registerUserInteractionModel(buttonEnabled.get());
      getHud()->registerUserInteractionModel(buttonOverlay.get());

      m_branchRadioGroup->addButton(buttonOverlay);

      gridBranches->addChild(buttonEnabled, osgGaming::ColRow(0, i));
      gridBranches->addChild(buttonOverlay, osgGaming::ColRow(1, i));
    }

    m_branchRadioGroup->getSelectedButtonObservable()->addNotifyFunc([this](UIButton::Ptr button)
    {
      if (!button)
      {
        getGlobeOverviewWorld()->getGlobeModel()->clearHighlightedCountries();
        return;
      }

      DebugButton* dbutton = dynamic_cast<DebugButton*>(button.get());
      if (!dbutton || dbutton->getType() != DebugButton::OVERLAY)
        return;

      getGlobeOverviewWorld()->getGlobeModel()->setHighlightedSkillBranch(BranchType(dbutton->getId()));
    });

    // Skills
    int nskills = getGlobeOverviewWorld()->getSimulation()->getNumSkills();
    stackPanelSkills->getCells()->setNumCells(nskills + 1);
    stackPanelSkills->getCells()->setSizePolicy(nskills, UICells::AUTO);

    for (int i = 0; i < nskills; i++)
    {
      Skill::Ptr skill = getGlobeOverviewWorld()->getSimulation()->getSkill(i);

      osg::ref_ptr<UIButton> button = new DebugButton(DebugButton::SKILL, i);
      button->setText(skill->getName());
      button->setCheckable(true);

      getHud()->registerUserInteractionModel(button.get());
      stackPanelSkills->addChild(button, i);
    }
  }

  void GlobeInteractionState::onMousePressedEvent(int button, float x, float y)
  {
    if (!ready())
    {
      return;
    }

    if (button == GUIEventAdapter::LEFT_MOUSE_BUTTON)
    {
      Vec3f point, direction;
      getWorld()->getCameraManipulator()->getPickRay(x, y, point, direction);

      Vec3f pickResult;
      if (sphereLineIntersection(Vec3f(0.0f, 0.0f, 0.0f), ~_paramEarthRadius, point, direction, pickResult))
      {
        Vec2f polar = getPolarFromCartesian(pickResult);
        ref_ptr<CountryMesh> countryMesh = getGlobeOverviewWorld()->getGlobeModel()->getCountryMesh(polar);

        int selected = countryMesh == nullptr ? 0 : int(countryMesh->getCountryData()->getId());

        getGlobeOverviewWorld()->getGlobeModel()->setSelectedCountry(selected);
        m_branchRadioGroup->reset();

        if (!_started)
        {
          if (selected == 0)
          {
            _textConfirm->setVisible(false);
          }
          else
          {
            if (_selectedCountry == selected)
            {
              startSimulation();
            }
            else
            {
              _textConfirm->setVisible(true);
            }
          }
        }

        _selectedCountry = selected;
      }
    }
  }

  void GlobeInteractionState::onKeyPressedEvent(int key)
  {
    /*if (!ready())
    {
    return;
    }*/

    if (_started)
    {
      if (key == GUIEventAdapter::KEY_Space)
      {
        if (_simulationTimer->running())
        {
          _simulationTimer->stop();

          printf("Simulation stopped\n");
        }
        else
        {
          _simulationTimer->start();

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
    if (!ready())
    {
      return;
    }

    float distance = getCameraDistance();

    if (motion == GUIEventAdapter::SCROLL_UP)
    {
      distance = distance * ~_paramCameraZoomSpeed;
    }
    else if (motion == GUIEventAdapter::SCROLL_DOWN)
    {
      distance = distance * (1.0f / ~_paramCameraZoomSpeed);
    }
    else
    {
      return;
    }

    distance = clampBetween(distance, ~_paramCameraMinDistance, ~_paramCameraMaxDistance);

    setCameraDistance(distance, getSimulationTime());
  }

  void GlobeInteractionState::onDragEvent(int button, Vec2f origin, Vec2f position, osg::Vec2f change)
  {
    if (!ready())
    {
      return;
    }

    Vec2f latLong = getCameraLatLong();
    float distance = getCameraDistance();
    Vec2f viewAngle = getCameraViewAngle();

    if (button == GUIEventAdapter::RIGHT_MOUSE_BUTTON)
    {
      change *= ((distance - ~_paramEarthRadius) / (~_paramCameraMaxDistance - ~_paramEarthRadius)) * ~_paramCameraZoomSpeedFactor;

      latLong.set(
        clampBetween(latLong.x() - change.y() * ~_paramCameraScrollSpeed, -~_paramCameraMaxLatitude, ~_paramCameraMaxLatitude),
        latLong.y() - change.x() * ~_paramCameraScrollSpeed);

      setCameraLatLong(latLong, getSimulationTime());
    }
    else if (button == GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
    {
      float clamp_to = atan(~_paramEarthRadius * 1.3 / distance);

      viewAngle.set(
        viewAngle.x() + change.x() * ~_paramCameraRotationSpeed,
        clampBetween(viewAngle.y() + change.y() * ~_paramCameraRotationSpeed, 0.0f, clamp_to));

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
      getGlobeOverviewWorld()->getSimulation()->getSkill(dbutton->getId())->setActivated(dbutton->isChecked());
    }
  }

  void GlobeInteractionState::dayTimerElapsed()
  {
    getGlobeOverviewWorld()->getSimulation()->step();
    // getGlobeOverviewWorld()->getSimulation()->printStats(true);

    _textProgress->setText(~Property<string, Param_LocalizationInfoTextDay>() + " " + to_string(getGlobeOverviewWorld()->getSimulation()->getDay()));

    updateCountryInfoText();
  }

  void GlobeInteractionState::startSimulation()
  {
    _textPleaseSelect->setVisible(false);
    _textConfirm->setVisible(false);
    _textProgress->setVisible(true);

    getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryMesh()->getCountryData()->setSkillBranchActivated(BRANCH_BANKS, true);

    _simulationTimer = TimerFactory::getInstance()->create<GlobeInteractionState>(&GlobeInteractionState::dayTimerElapsed, this, 1.0, false);
    _simulationTimer->start();

    _started = true;
  }

  /*void GlobeInteractionState::onUIMClickedEvent(UserInteractionModel* model)
  {
  printf("Clicked UIM: %s\n", model->getUIMName().data());
  }*/

  ref_ptr<Hud> GlobeInteractionState::newHud()
  {
    return new Hud();
  }

  bool GlobeInteractionState::ready()
  {
    if (!_ready && !isCameraInMotion())
    {
      setCameraMotionDuration(0.5);
      setCameraMotionEase(AnimationEase::CIRCLE_OUT);
      _ready = true;
    }

    return _ready;
  }

  void GlobeInteractionState::updateCountryInfoText()
  {
    if (getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryId() == 0)
    {
      _textCountryInfo->setText("");

      return;
    }

    string infoText = "";
    CountryData::Ptr country = getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountryMesh()->getCountryData();

    //char buffer[16];

    infoText += country->getCountryName() + "\n";
    /*infoText += "Wealth: " + to_string(country->getWealth()) + "\n";

    sprintf(buffer, "%.2f", country->getDept());
    infoText += "Dept: " + string(buffer) + "\n";

    sprintf(buffer, "%.2f", country->getRelativeDept() * 100.0f);
    infoText += "Rel. Dept: " + string(buffer) + "%\n";

    sprintf(buffer, "%.2f", country->getDeptBalance());
    infoText += "Dept balance: " + string(buffer) + "\n";

    sprintf(buffer, "%.2f", country->getAnger() * 100.0f);
    infoText += "Anger: " + string(buffer) + "%\n";

    sprintf(buffer, "%.2f", country->getAngerBalance());
    infoText += "Anger balance: " + string(buffer) + "\n";*/


    country->getValues()->getContainer()->debugPrintToString(infoText);

    _textCountryInfo->setText(infoText);
  }

}