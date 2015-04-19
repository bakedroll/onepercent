#pragma once

#include "World.h"
#include "GameSettings.h"

#include <osg/Referenced>
#include <osg/ref_ptr>

namespace osgGaming
{
	class GameState;

	typedef enum _stateEventType
	{
		PUSH,
		POP,
		REPLACE,
		END_GAME
	} StateEventType;

	typedef struct _stateEvent
	{
		StateEventType type;
		osg::ref_ptr<GameState> referencedState;
	} StateEvent;

	class GameState : public osg::Referenced
	{
	public:
		virtual StateEvent* update(double frameTime, osg::ref_ptr<World> world, osg::ref_ptr<GameSettings> settings);

	protected:
		StateEvent* stateEvent_push(osg::ref_ptr<GameState> state);
		StateEvent* stateEvent_pop();
		StateEvent* stateEvent_replace(osg::ref_ptr<GameState> state);
		StateEvent* stateEvent_endGame();
	};
}