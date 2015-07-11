#include "GlobeInteractionState.h"
#include "GlobeModel.h"

#include <osgGaming/Helper.h>
#include <osgGaming/UIStackPanel.h>
#include <osgGaming/TimerFactory.h>

using namespace onep;
using namespace osg;
using namespace std;
using namespace osgGA;
using namespace osgGaming;

GlobeInteractionState::GlobeInteractionState()
	: GlobeCameraState(),
	  _ready(false),
	  _started(false),
	  _selectedCountry(255)
{
}

void GlobeInteractionState::initialize()
{
	GlobeCameraState::initialize();

	ref_ptr<UIStackPanel> stackPanel = new UIStackPanel();
	stackPanel->setOrientation(UIStackPanel::VERTICAL);
	stackPanel->getCells()->setNumCells(4);
	stackPanel->getCells()->setSizePolicy(0, UICells::CONTENT);
	stackPanel->getCells()->setSizePolicy(2, UICells::CONTENT);
	stackPanel->getCells()->setSizePolicy(3, UICells::CONTENT);
	stackPanel->setMargin(10.0f);

	_textPleaseSelect = new UIText();
	_textPleaseSelect->setText("Please select a country.");

	_textConfirm = new UIText();
	_textConfirm->setText("Click again to confirm your selection.");
	_textConfirm->setFontSize(14);
	_textConfirm->setVisible(false);

	_textProgress = new UIText();
	_textProgress->setText("Day 0");
	_textProgress->setFontSize(32); //(14);
	_textProgress->setMargin(Vec4f(0.0f, 0.0f, 0.0f, 30.0f));
	_textProgress->setVisible(false);

	//text->setMargin(20.0f);

	getHud()->getRootUIElement()->addChild(stackPanel);
	stackPanel->addChild(_textPleaseSelect, 3);
	stackPanel->addChild(_textConfirm, 2);
	stackPanel->addChild(_textProgress, 0);

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
			ref_ptr<Country> country = getGlobeOverviewWorld()->getSimulation()->getCountry(polar);

			int selected = country == NULL ? 255 : (int)country->getId();

			getGlobeOverviewWorld()->getGlobeModel()->setSelectedCountry(selected);

			if (!_started)
			{
				if (selected == 255)
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

			if (country != NULL)
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






				/*float angle = (float)getWorld()->getCameraManipulator()->getProjectionAngle();
				float ratio = (float)getWorld()->getCameraManipulator()->getProjectionRatio();

				float hdistance = surfaceSize.x() * 1.5f / (2.0f * tan(angle * ratio * C_PI / 360.0f)) + GlobeModel::EARTH_RADIUS;
				float vdistance = surfaceSize.y() * 1.5f / (2.0f * tan(angle * C_PI / 360.0f)) + GlobeModel::EARTH_RADIUS;

				float distance = max(_MIN_CAMERA_DISTANCE, max(hdistance, vdistance));*/

				setCameraLatLong(country->getCenterLatLong(), getSimulationTime());
				setCameraDistance(max(country->getOptimalCameraDistance(
					(float)getWorld()->getCameraManipulator()->getProjectionAngle(),
					(float)getWorld()->getCameraManipulator()->getProjectionRatio()), ~_paramCameraMinDistance), getSimulationTime());
			}
			
			//printf("INTERSECTION at %f, %f, %f Polar: %f, %f Id: %d Country: %s\n", pickResult.x(), pickResult.y(), pickResult.z(), polar.x(), polar.y(), _selectedCountry, country_name.data());
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
		else if (key == GUIEventAdapter::KEY_P)
		{
			getGlobeOverviewWorld()->getSimulation()->printStats();
		}
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

void GlobeInteractionState::dayTimerElapsed()
{
	char textProgressBuffer[16];

	getGlobeOverviewWorld()->getSimulation()->step();
	getGlobeOverviewWorld()->getSimulation()->printStats(true);

	sprintf(&textProgressBuffer[0], "Day %d\n", getGlobeOverviewWorld()->getSimulation()->getDay());
	_textProgress->setText(textProgressBuffer);
}

void GlobeInteractionState::startSimulation()
{
	_textPleaseSelect->setVisible(false);
	_textConfirm->setVisible(false);
	_textProgress->setVisible(true);

	getGlobeOverviewWorld()->getSimulation()->getCountry(
		(unsigned char)getGlobeOverviewWorld()->getGlobeModel()->getSelectedCountry())
		->setSkillBranchActivated(Country::BRANCH_BANKS, true);

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