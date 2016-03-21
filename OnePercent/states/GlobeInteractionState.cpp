#include "GlobeInteractionState.h"

#include "nodes/GlobeModel.h"

#include <osgGaming/Helper.h>
#include <osgGaming/TimerFactory.h>
#include <osgGaming/UIStackPanel.h>
#include <osgGaming/UIButton.h>

using namespace onep;
using namespace osg;
using namespace std;
using namespace osgGA;
using namespace osgGaming;

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

	_textPleaseSelect = static_cast<UIText*>(getHud()->getUIElementByName("text_selectCountry").get());
	_textConfirm = static_cast<UIText*>(getHud()->getUIElementByName("text_selectCountryAgain").get());
	_textProgress = static_cast<UIText*>(getHud()->getUIElementByName("text_dayProgress").get());
	_textCountryInfo = static_cast<UIText*>(getHud()->getUIElementByName("text_countryInfo").get());

  UIStackPanel* stackPanel = static_cast<UIStackPanel*>(getHud()->getUIElementByName("panel_skillbuttons").get());

  int nskills = getGlobeOverviewWorld()->getSimulation()->getNumSkills();
  stackPanel->getCells()->setNumCells(nskills + 1);
  stackPanel->getCells()->setSizePolicy(0, UICells::AUTO);

  for (int i = 0; i < nskills; i++)
  {
    Skill::Ptr skill = getGlobeOverviewWorld()->getSimulation()->getSkill(i);
    
    osg::ref_ptr<UIButton> button = new UIButton();
    button->setText(skill->getName());

    stackPanel->addChild(button, nskills - i);
  }

	getHud()->setFpsEnabled(true);

	setCameraMotionDuration(2.0);
	setCameraMotionEase(AnimationEase::SMOOTHER);

	setCameraDistance(28.0f, getSimulationTime());
	setCameraViewAngle(Vec2f(0.0f, 0.0f), getSimulationTime());
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

			updateCountryInfoText();

			if (countryMesh != nullptr)
			{
				//Vec2f countrySize = country->getSize();
				//Vec2f surfaceSize = country->getSurfaceSize();
				//Vec2f latLong = country->getCenterLatLong();



				/*ref_ptr<PositionAttitudeTransform> rotation = new PositionAttitudeTransform();
				rotation->setAttitude(getQuatFromEuler(latLong.x(), 0.0f, latLong.y()));

				ref_ptr<PositionAttitudeTransform> transform = new PositionAttitudeTransform();
				transform->setPosition(Vec3f(0.0f, GlobeModel::EARTH_RADIUS , 0.0f));
				transform->setScale(Vec3f(surfaceSize.x(), 1.0f, surfaceSize.y()));

				ref_ptr<Geode> geode = new Geode();
				geode->addDrawable(createQuadGeometry(-0.5f, 0.5f, -0.5f, 0.5f, 0.0f, XZ, true));

				rotation->addChild(transform);
				transform->addChild(geode);
				getWorld()->getRootNode()->addChild(rotation);*/



				printf("Selected country (%d): %s\n", _selectedCountry, countryMesh->getCountryData()->getCountryName().c_str());

        setCameraLatLong(countryMesh->getCountryData()->getCenterLatLong(), getSimulationTime());
        setCameraDistance(max(countryMesh->getCountryData()->getOptimalCameraDistance(
					float(getWorld()->getCameraManipulator()->getProjectionAngle()),
					float(getWorld()->getCameraManipulator()->getProjectionRatio())), ~_paramCameraMinDistance), getSimulationTime());
			}
			
			
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
  if (_selectedCountry > 0)
  {
    ref_ptr<CountryData> selectedCountry = getGlobeOverviewWorld()->getGlobeModel()->getCountryMesh(_selectedCountry)->getCountryData();

    if (uiElement->getUIName() == "button_makeBanks")
    {
      if (selectedCountry)
        selectedCountry->setSkillBranchActivated(BRANCH_BANKS, true);
    }
    else if (uiElement->getUIName() == "button_makePolitics")
    {
      if (selectedCountry)
        selectedCountry->setSkillBranchActivated(BRANCH_POLITICS, true);
    }
    else if (uiElement->getUIName() == "button_makeConcerns")
    {
      if (selectedCountry)
        selectedCountry->setSkillBranchActivated(BRANCH_CONCERNS, true);
    }
    else if (uiElement->getUIName() == "button_makeMedia")
    {
      if (selectedCountry)
        selectedCountry->setSkillBranchActivated(BRANCH_MEDIA, true);
    }
    else if (uiElement->getUIName() == "button_makeControl")
    {
      if (selectedCountry)
        selectedCountry->setSkillBranchActivated(BRANCH_CONTROL, true);
    }
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

  getGlobeOverviewWorld()->getGlobeModel()->getCountryMesh(_selectedCountry)->getCountryData()->setSkillBranchActivated(BRANCH_BANKS, true);

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
	if (_selectedCountry == 0)
	{
		_textCountryInfo->setText("");

		return;
	}

	string infoText = "";
  CountryData::Ptr country = getGlobeOverviewWorld()->getGlobeModel()->getCountryMesh(_selectedCountry)->getCountryData();

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