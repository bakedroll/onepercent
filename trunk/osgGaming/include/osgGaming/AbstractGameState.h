#pragma once

#include <vector>

#include <osgGaming/World.h>
#include <osgGaming/Hud.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/UserInteractionModel.h>
#include <osgGaming/Viewer.h>

#include <osg/Referenced>
#include <osg/ref_ptr>

namespace osgGaming
{
	class AbstractGameState : public osg::Referenced
	{
	public:
		typedef enum _stateEventType
		{
			PUSH,
			POP,
			REPLACE,
			END_GAME
		} StateEventType;

		typedef enum _stateProperties
		{
			PROP_ENABLED = 0x01,
			PROP_RUN_ALWAYS = 0x02
		} StateProperties;

		typedef std::vector<osg::ref_ptr<AbstractGameState>> AbstractGameStateList;

		typedef struct _stateEvent
		{
			StateEventType type;
			AbstractGameStateList referencedStates;
		} StateEvent;

		AbstractGameState();

		bool isInitialized();
		bool isWorldAndHudPrepared();
		void setInitialized();

		virtual void initialize();
		virtual StateEvent* update();

		virtual bool isLoadingState() = 0;
		virtual unsigned char getProperties();

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

		void prepareWorldAndHud();

		double getSimulationTime();
		double getFrameTime();
		osg::ref_ptr<World> getWorld();
		osg::ref_ptr<Hud> getHud();
		osg::ref_ptr<Viewer> getViewer();
		osg::ref_ptr<GameSettings> getGameSettings();

		void setSimulationTime(double simulationTime);
		void setFrameTime(double frameTime);
		void setWorld(osg::ref_ptr<World> world);
		void setHud(osg::ref_ptr<Hud> hud);
		void setViewer(osg::ref_ptr<Viewer> viewer);
		void setGameSettings(osg::ref_ptr<GameSettings> settings);

	protected:
		virtual osg::ref_ptr<World> newWorld();
		virtual osg::ref_ptr<Hud> newHud();

		StateEvent* stateEvent_push(osg::ref_ptr<AbstractGameState> state);
		StateEvent* stateEvent_push(AbstractGameStateList states);
		StateEvent* stateEvent_pop();
		StateEvent* stateEvent_replace(osg::ref_ptr<AbstractGameState> state);
		StateEvent* stateEvent_replace(AbstractGameStateList states);
		StateEvent* stateEvent_endGame();

		StateEvent* stateEvent_default();

	private:
		bool _initialized;
		bool _worldHudPrepared;

		StateEvent* _stateEvent;

		double _simulationTime;
		double _frameTime;
		osg::ref_ptr<World> _world;
		osg::ref_ptr<Hud> _hud;
		osg::ref_ptr<Viewer> _viewer;
		osg::ref_ptr<GameSettings> _gameSettings;
	};
}