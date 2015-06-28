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

const float GlobeInteractionState::_MIN_CAMERA_DISTANCE = 9.0f;
const float GlobeInteractionState::_MAX_CAMERA_DISTANCE = 100.0f;
const float GlobeInteractionState::_MAX_CAMERA_LONGITUDE = C_PI / 2.0f * 0.9f;
const float GlobeInteractionState::_CAMERA_ZOOM_SPEED = 0.85f;
const float GlobeInteractionState::_CAMERA_ZOOM_SPEED_FACTOR = 3.0f;
const float GlobeInteractionState::_CAMERA_SCROLL_SPEED = 0.003f;
const float GlobeInteractionState::_CAMERA_ROTATION_SPEED = 0.003;

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
	_textProgress->setFontSize(14);
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
		if (sphereLineIntersection(Vec3f(0.0f, 0.0f, 0.0f), GlobeModel::EARTH_RADIUS, point, direction, pickResult))
		{
			Vec2f polar = getPolarFromCartesian(pickResult);
			int selected = (int)getGlobeOverviewWorld()->getSimulation()->getCountryId(polar);
			//string country_name = getGlobeOverviewWorld()->getSimulation()->getCountryName(polar);

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
		distance = distance * _CAMERA_ZOOM_SPEED;
	}
	else if (motion == GUIEventAdapter::SCROLL_DOWN)
	{
		distance = distance * (1.0f / _CAMERA_ZOOM_SPEED);
	}
	else
	{
		return;
	}

	distance = clamp(distance, _MIN_CAMERA_DISTANCE, _MAX_CAMERA_DISTANCE);

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
		change *= ((distance - GlobeModel::EARTH_RADIUS) / (_MAX_CAMERA_DISTANCE - GlobeModel::EARTH_RADIUS)) * _CAMERA_ZOOM_SPEED_FACTOR;

		latLong.set(
			clamp(latLong.x() - change.y() * _CAMERA_SCROLL_SPEED, -_MAX_CAMERA_LONGITUDE, _MAX_CAMERA_LONGITUDE),
			latLong.y() - change.x() * _CAMERA_SCROLL_SPEED);

		setCameraLatLong(latLong, getSimulationTime());
	}
	else if (button == GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
	{
		float clamp_to = atan(GlobeModel::EARTH_RADIUS * 1.3 / distance);

		viewAngle.set(
			viewAngle.x() + change.x() * _CAMERA_ROTATION_SPEED,
			clamp(viewAngle.y() + change.y() * _CAMERA_ROTATION_SPEED, 0.0f, clamp_to));

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

	sprintf(&textProgressBuffer[0], "Day %d\n", getGlobeOverviewWorld()->getSimulation()->getDay());
	_textProgress->setText(textProgressBuffer);
}

void GlobeInteractionState::startSimulation()
{
	_textPleaseSelect->setVisible(false);
	_textConfirm->setVisible(false);
	_textProgress->setVisible(true);

	_simulationTimer = TimerFactory::get()->create<GlobeInteractionState>(&GlobeInteractionState::dayTimerElapsed, this, 1.0, false);
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