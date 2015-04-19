#include "GameState.h"

using namespace osgGaming;
using namespace osg;

StateEvent* GameState::update(double frameTime, ref_ptr<World> world, ref_ptr<GameSettings> settings)
{
	return NULL;
}

StateEvent* GameState::stateEvent_push(ref_ptr<GameState> state)
{
	StateEvent* e = new StateEvent();
	e->type = PUSH;
	e->referencedState = state;

	return e;
}

StateEvent* GameState::stateEvent_pop()
{
	StateEvent* e = new StateEvent();
	e->type = POP;

	return e;
}

StateEvent* GameState::stateEvent_replace(ref_ptr<GameState> state)
{
	StateEvent* e = new StateEvent();
	e->type = REPLACE;
	e->referencedState = state;

	return e;
}

StateEvent* GameState::stateEvent_endGame()
{
	StateEvent* e = new StateEvent();
	e->type = END_GAME;

	return e;
}