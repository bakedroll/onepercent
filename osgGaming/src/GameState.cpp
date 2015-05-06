#include <osgGaming/GameState.h>

using namespace osgGaming;
using namespace osg;

GameState::GameState()
	: Referenced(),
	  _initialized(false),
	  _stateEvent(NULL)
{

}

bool GameState::isInitialized()
{
	return _initialized;
}

void GameState::setInitialized()
{
	_initialized = true;
}

void GameState::initialize(float resolutionWidth, float resolutionHeight)
{

}

StateEvent* GameState::update()
{
	return stateEvent_default();
}

bool GameState::isLoadingState()
{
	return false;
}

void GameState::onKeyPressedEvent(int key)
{

}

void GameState::onKeyReleasedEvent(int key)
{

}

void GameState::onMousePressedEvent(int button, float x, float y)
{

}

void GameState::onMouseReleasedEvent(int button, float x, float y)
{

}

void GameState::onMouseMoveEvent(float x, float y)
{

}

void GameState::onScrollEvent(osgGA::GUIEventAdapter::ScrollingMotion motion)
{

}

void GameState::onDragEvent(int button, Vec2f origin, Vec2f position, osg::Vec2f change)
{

}

void GameState::onDragBeginEvent(int button, Vec2f origin)
{

}

void GameState::onDragEndEvent(int button, Vec2f origin, Vec2f position)
{

}

void GameState::onUIMClickedEvent(UserInteractionModel* model)
{

}

void GameState::onResizeEvent(float width, float height)
{

}

double GameState::getSimulationTime()
{
	return _simulationTime;
}

double GameState::getFrameTime()
{
	return _frameTime;
}

ref_ptr<World> GameState::getWorld()
{
	return _world;
}

ref_ptr<GameSettings> GameState::getGameSettings()
{
	return _gameSettings;
}

void GameState::setSimulationTime(double simulationTime)
{
	_simulationTime = simulationTime;
}

void GameState::setFrameTime(double frameTime)
{
	_frameTime = frameTime;
}

void GameState::setWorld(ref_ptr<World> world)
{
	_world = world;
}

void GameState::setGameSettings(ref_ptr<GameSettings> settings)
{
	_gameSettings = settings;
}

StateEvent* GameState::stateEvent_push(ref_ptr<GameState> state)
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

StateEvent* GameState::stateEvent_pop()
{
	if (_stateEvent != NULL)
	{
		return _stateEvent;
	}

	_stateEvent = new StateEvent();
	_stateEvent->type = POP;

	return _stateEvent;
}

StateEvent* GameState::stateEvent_replace(ref_ptr<GameState> state)
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

StateEvent* GameState::stateEvent_endGame()
{
	if (_stateEvent != NULL)
	{
		return _stateEvent;
	}

	_stateEvent = new StateEvent();
	_stateEvent->type = END_GAME;

	return _stateEvent;
}

StateEvent* GameState::stateEvent_default()
{
	return _stateEvent;
}