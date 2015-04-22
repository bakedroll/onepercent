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

void GameState::initialize(osg::ref_ptr<World> world, osg::ref_ptr<GameSettings> settings)
{

}

StateEvent* GameState::update(double frameTime, ref_ptr<World> world, ref_ptr<GameSettings> settings)
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

void GameState::onDragEvent(int button, osg::Vec2f origin, osg::Vec2f position)
{

}

void GameState::onDragBeginEvent(int button, osg::Vec2f origin)
{

}

void GameState::onDragEndEvent(int button, osg::Vec2f origin, osg::Vec2f position)
{

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