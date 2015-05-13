#pragma once

#include <osgGaming/World.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/UserInteractionModel.h>
#include <osgGaming/Viewer.h>

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

		virtual void initialize(float resolutionWidth, float resolutionHeight);
		virtual StateEvent* update();

		virtual bool isLoadingState();

		virtual void onKeyPressedEvent(int key);
		virtual void onKeyReleasedEvent(int key);

		virtual void onMousePressedEvent(int button, float x, float y);
		virtual void onMouseReleasedEvent(int button, float x, float y);

		virtual void onMouseMoveEvent(float x, float y);

		virtual void onScrollEvent(osgGA::GUIEventAdapter::ScrollingMotion motion);

		virtual void onDragEvent(int button, osg::Vec2f origin, osg::Vec2f position, osg::Vec2f change);
		virtual void onDragBeginEvent(int button, osg::Vec2f origin);
		virtual void onDragEndEvent(int button, osg::Vec2f origin, osg::Vec2f position);

		virtual void onUIMClickedEvent(UserInteractionModel* model);

		virtual void onResizeEvent(float width, float height);

		double getSimulationTime();
		double getFrameTime();
		osg::ref_ptr<World> getWorld();
		osg::ref_ptr<Viewer> getViewer();
		osg::ref_ptr<GameSettings> getGameSettings();

		void setSimulationTime(double simulationTime);
		void setFrameTime(double frameTime);
		void setWorld(osg::ref_ptr<World> world);
		void setViewer(osg::ref_ptr<Viewer> viewer);
		void setGameSettings(osg::ref_ptr<GameSettings> settings);

	protected:
		StateEvent* stateEvent_push(osg::ref_ptr<GameState> state);
		StateEvent* stateEvent_pop();
		StateEvent* stateEvent_replace(osg::ref_ptr<GameState> state);
		StateEvent* stateEvent_endGame();

		StateEvent* stateEvent_default();

	private:
		bool _initialized;

		StateEvent* _stateEvent;

		double _simulationTime;
		double _frameTime;
		osg::ref_ptr<World> _world;
		osg::ref_ptr<Viewer> _viewer;
		osg::ref_ptr<GameSettings> _gameSettings;
	};
}