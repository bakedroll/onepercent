#include <osgGaming/AbstractGameState.h>

using namespace osgGaming;
using namespace osg;

AbstractGameState::AbstractGameState()
	: Referenced(),
	  _initialized(false),
	  _worldHudPrepared(false),
	  _stateEvent(NULL),
	  _firstUpdate(true)
{
	for (int i = 0; i < _stateBehaviorCount; i++)
	{
		_dirty[i] = true;
	}
}

bool AbstractGameState::isInitialized()
{
	return _initialized;
}

bool AbstractGameState::isWorldAndHudPrepared()
{
	return _worldHudPrepared;
}

bool AbstractGameState::isFirstUpdate()
{
	bool first = _firstUpdate;

	if (first)
	{
		_firstUpdate = false;
	}

	return first;
}

void AbstractGameState::setInitialized()
{
	_initialized = true;
}

void AbstractGameState::dirty(StateBehavior behavior)
{
	_dirty[(int)behavior] = true;
}

bool AbstractGameState::isDirty(StateBehavior behavior)
{
	bool dirty = _dirty[(int)behavior];

	_dirty[(int)behavior] = false;

	return dirty;
}

void AbstractGameState::initialize()
{

}

AbstractGameState::StateEvent* AbstractGameState::update()
{
	return stateEvent_default();
}

unsigned char AbstractGameState::getProperties()
{
	return AbstractGameState::PROP_GUIEVENTS_TOP | AbstractGameState::PROP_UIMEVENTS_TOP | AbstractGameState::PROP_UPDATE_TOP;
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

void AbstractGameState::onUIClickedEvent(osg::ref_ptr<UIElement> uiElement)
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

AbstractGameState::StateEvent* AbstractGameState::stateEvent_default()
{
	return _stateEvent;
}

ref_ptr<World> AbstractGameState::newWorld()
{
	return NULL;
}

ref_ptr<Hud> AbstractGameState::newHud()
{
	return NULL;
}

AbstractGameState::StateEvent* AbstractGameState::stateEvent_push(ref_ptr<AbstractGameState> state)
{
	if (_stateEvent != NULL)
	{
		return _stateEvent;
	}

	_stateEvent = new StateEvent();
	_stateEvent->type = PUSH;
	_stateEvent->referencedStates.push_back(state);

	return _stateEvent;
}

AbstractGameState::StateEvent* AbstractGameState::stateEvent_push(AbstractGameStateList states)
{
	if (_stateEvent != NULL)
	{
		return _stateEvent;
	}

	_stateEvent = new StateEvent();
	_stateEvent->type = PUSH;
	_stateEvent->referencedStates = states;

	return _stateEvent;
}

AbstractGameState::StateEvent* AbstractGameState::stateEvent_pop()
{
	if (_stateEvent != NULL)
	{
		return _stateEvent;
	}

	_stateEvent = new StateEvent();
	_stateEvent->type = POP;

	return _stateEvent;
}

AbstractGameState::StateEvent* AbstractGameState::stateEvent_replace(ref_ptr<AbstractGameState> state)
{
	if (_stateEvent != NULL)
	{
		return _stateEvent;
	}

	_stateEvent = new StateEvent();
	_stateEvent->type = REPLACE;
	_stateEvent->referencedStates.push_back(state);

	return _stateEvent;
}

AbstractGameState::StateEvent* AbstractGameState::stateEvent_replace(AbstractGameStateList states)
{
	if (_stateEvent != NULL)
	{
		return _stateEvent;
	}

	_stateEvent = new StateEvent();
	_stateEvent->type = REPLACE;
	_stateEvent->referencedStates = states;

	return _stateEvent;
}

AbstractGameState::StateEvent* AbstractGameState::stateEvent_endGame()
{
	if (_stateEvent != NULL)
	{
		return _stateEvent;
	}

	_stateEvent = new StateEvent();
	_stateEvent->type = END_GAME;

	return _stateEvent;
}