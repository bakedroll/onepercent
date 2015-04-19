#pragma once

#include "osgGaming/World.h"
#include "osgGaming/GameSettings.h"

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
		GameState();

		bool isInitialized();
		void setInitialized();

		virtual void initialize(osg::ref_ptr<World> world, osg::ref_ptr<GameSettings> settings);
		virtual StateEvent* update(double frameTime, osg::ref_ptr<World> world, osg::ref_ptr<GameSettings> settings);

		virtual bool isLoadingState();

	protected:
		StateEvent* stateEvent_push(osg::ref_ptr<GameState> state);
		StateEvent* stateEvent_pop();
		StateEvent* stateEvent_replace(osg::ref_ptr<GameState> state);
		StateEvent* stateEvent_endGame();

	private:
		bool _initialized;
	};
}