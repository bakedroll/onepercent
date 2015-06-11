#include <osgGaming/AbstractGameState.h>

using namespace osgGaming;
using namespace osg;

AbstractGameState::AbstractGameState()
	: Referenced(),
	  _initialized(false),
	  _worldHudPrepared(false),
	  _stateEvent(NULL)
{

}

bool AbstractGameState::isInitialized()
{
	return _initialized;
}

bool AbstractGameState::isWorldAndHudPrepared()
{
	return _worldHudPrepared;
}

void AbstractGameState::setInitialized()
{
	_initialized = true;
}

void AbstractGameState::initialize()
{

}

StateEvent* AbstractGameState::update()
{
	return stateEvent_default();
}

void AbstractGameState::onKeyPressedEvent(int key)
{

}

void AbstractGameState::onKeyReleasedEvent(int key)
{

}

void AbstractGameState::onMousePressedEvent(int button, float x, float y)
{

}

void AbstractGameState::onMouseReleasedEvent(int button, float x, float y)
{

}

void AbstractGameState::onMouseMoveEvent(float x, float y)
{

}

void AbstractGameState::onScrollEvent(osgGA::GUIEventAdapter::ScrollingMotion motion)
{

}

void AbstractGameState::onDragEvent(int button, Vec2f origin, Vec2f position, osg::Vec2f change)
{

}

void AbstractGameState::onDragBeginEvent(int button, Vec2f origin)
{

}

void AbstractGameState::onDragEndEvent(int button, Vec2f origin, Vec2f position)
{

}

void AbstractGameState::onUIMClickedEvent(UserInteractionModel* model)
{

}

void AbstractGameState::onResizeEvent(float width, float height)
{

}

void AbstractGameState::prepareWorldAndHud()
{
	setWorld(newWorld());
	setHud(newHud());

	_worldHudPrepared = true;
}

double AbstractGameState::getSimulationTime()
{
	return _simulationTime;
}

double AbstractGameState::getFrameTime()
{
	return _frameTime;
}

ref_ptr<World> AbstractGameState::getWorld()
{
	return _world;
}

ref_ptr<Hud> AbstractGameState::getHud()
{
	return _hud;
}

ref_ptr<Viewer> AbstractGameState::getViewer()
{
	return _viewer;
}

ref_ptr<GameSettings> AbstractGameState::getGameSettings()
{
	return _gameSettings;
}

void AbstractGameState::setSimulationTime(double simulationTime)
{
	_simulationTime = simulationTime;
}

void AbstractGameState::setFrameTime(double frameTime)
{
	_frameTime = frameTime;
}

void AbstractGameState::setWorld(ref_ptr<World> world)
{
	_world = world;
}

void AbstractGameState::setHud(ref_ptr<Hud> hud)
{
	_hud = hud;
}

void AbstractGameState::setViewer(ref_ptr<Viewer> viewer)
{
	_viewer = viewer;
}

void AbstractGameState::setGameSettings(ref_ptr<GameSettings> settings)
{
	_gameSettings = settings;
}

ref_ptr<World> AbstractGameState::newWorld()
{
	return NULL;
}

ref_ptr<Hud> AbstractGameState::newHud()
{
	return NULL;
}

StateEvent* AbstractGameState::stateEvent_push(ref_ptr<AbstractGameState> state)
{
	if (_stateEvent != NULL)
	{
		return _stateEvent;
	}

	_stateEvent = new StateEvent();
	_stateEvent->type = PUSH;
	_stateEvent->referencedState = state;

	return _stateEvent;
}

StateEvent* AbstractGameState::stateEvent_pop()
{
	if (_stateEvent != NULL)
	{
		return _stateEvent;
	}

	_stateEvent = new StateEvent();
	_stateEvent->type = POP;

	return _stateEvent;
}

StateEvent* AbstractGameState::stateEvent_replace(ref_ptr<AbstractGameState> state)
{
	if (_stateEvent != NULL)
	{
		return _stateEvent;
	}

	_stateEvent = new StateEvent();
	_stateEvent->type = REPLACE;
	_stateEvent->referencedState = state;

	return _stateEvent;
}

StateEvent* AbstractGameState::stateEvent_endGame()
{
	if (_stateEvent != NULL)
	{
		return _stateEvent;
	}

	_stateEvent = new StateEvent();
	_stateEvent->type = END_GAME;

	return _stateEvent;
}

StateEvent* AbstractGameState::stateEvent_default()
{
	return _stateEvent;
}