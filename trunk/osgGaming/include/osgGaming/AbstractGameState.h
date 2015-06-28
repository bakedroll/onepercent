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
			PROP_UPDATE_TOP = 0x01,
			PROP_UPDATE_ALWAYS = 0x02,
			PROP_GUIEVENTS_TOP = 0x04,
			PROP_GUIEVENTS_ALWAYS = 0x08,
			PROP_UIMEVENTS_TOP = 0x16,
			PROP_UIMEVENTS_ALWAYS = 0x32
		} StateProperties;

		typedef enum _stateBahavior
		{
			UPDATE,
			GUIEVENT,
			UIMEVENT,
			ALL
		} StateBehavior;

		typedef std::vector<osg::ref_ptr<AbstractGameState>> AbstractGameStateList;

		typedef struct _stateEvent
		{
			StateEventType type;
			AbstractGameStateList referencedStates;
		} StateEvent;

		AbstractGameState();

		bool isInitialized();
		bool isWorldAndHudPrepared();
		bool isFirstUpdate();
		void setInitialized();

		void dirty(StateBehavior behavior);
		bool isDirty(StateBehavior behavior);

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

		StateEvent* stateEvent_default();

	protected:
		virtual osg::ref_ptr<World> newWorld();
		virtual osg::ref_ptr<Hud> newHud();

		StateEvent* stateEvent_push(osg::ref_ptr<AbstractGameState> state);
		StateEvent* stateEvent_push(AbstractGameStateList states);
		StateEvent* stateEvent_pop();
		StateEvent* stateEvent_replace(osg::ref_ptr<AbstractGameState> state);
		StateEvent* stateEvent_replace(AbstractGameStateList states);
		StateEvent* stateEvent_endGame();

	private:
		static const int _stateBehaviorCount = 3;

		bool _initialized;
		bool _firstUpdate;
		bool _worldHudPrepared;
		bool _dirty[_stateBehaviorCount];

		StateEvent* _stateEvent;

		double _simulationTime;
		double _frameTime;
		osg::ref_ptr<World> _world;
		osg::ref_ptr<Hud> _hud;
		osg::ref_ptr<Viewer> _viewer;
		osg::ref_ptr<GameSettings> _gameSettings;

	};
}