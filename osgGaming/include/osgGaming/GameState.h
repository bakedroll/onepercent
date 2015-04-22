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

		virtual void onKeyPressedEvent(int key);
		virtual void onKeyReleasedEvent(int key);

		virtual void onMousePressedEvent(int button, float x, float y);
		virtual void onMouseReleasedEvent(int button, float x, float y);

		virtual void onMouseMoveEvent(float x, float y);

		virtual void onDragEvent(int button, osg::Vec2f origin, osg::Vec2f position);
		virtual void onDragBeginEvent(int button, osg::Vec2f origin);
		virtual void onDragEndEvent(int button, osg::Vec2f origin, osg::Vec2f position);

	protected:
		StateEvent* stateEvent_push(osg::ref_ptr<GameState> state);
		StateEvent* stateEvent_pop();
		StateEvent* stateEvent_replace(osg::ref_ptr<GameState> state);
		StateEvent* stateEvent_endGame();

		StateEvent* stateEvent_default();

	private:
		bool _initialized;

		StateEvent* _stateEvent;
	};
}