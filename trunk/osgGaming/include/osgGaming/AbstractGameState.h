#pragma once

#include "Viewer.h"

#include <vector>
#include <memory>

#include <osgGaming/Injector.h>

#include <osg/Referenced>
#include <osg/ref_ptr>

namespace osgGaming
{
  class GameSettings;
  class Hud;
  class NativeView;
  class View;
  class World;

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
			PROP_UPDATE_TOP = 1,
			PROP_UPDATE_ALWAYS = 2,
			PROP_GUIEVENTS_TOP = 4,
			PROP_GUIEVENTS_ALWAYS = 8
		} StateProperties;

		typedef enum _stateBahavior
		{
			UPDATE,
			GUIEVENT,
			ALL
		} StateBehavior;

		typedef std::vector<osg::ref_ptr<AbstractGameState>> AbstractGameStateList;
    typedef std::vector<std::reference_wrapper<osg::ref_ptr<AbstractGameState>>> AbstractGameStateRefList;

		typedef struct _stateEvent
		{
			StateEventType type;
			AbstractGameStateList referencedStates;
		} StateEvent;

		AbstractGameState();
	  virtual ~AbstractGameState();

		bool isInitialized();
		bool isWorldAndHudPrepared();
    void setWorldAndHudPrepared();
		bool isFirstUpdate();
		void setInitialized();

		void setDirty(StateBehavior behavior);
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

		virtual void onResizeEvent(float width, float height);

    void prepareWorldAndHud(osgGaming::Injector& injector, osg::ref_ptr<View> view);

		double getSimulationTime();
		double getFrameTime();
    osg::ref_ptr<World> getWorld(osg::ref_ptr<View> view);
    osg::ref_ptr<Hud> getHud(osg::ref_ptr<View> view);
    osg::ref_ptr<osgGaming::Viewer> getViewer();
    osg::ref_ptr<osgGaming::View> getView(int i);
		osg::ref_ptr<GameSettings> getGameSettings();

		void setSimulationTime(double simulationTime);
		void setFrameTime(double frameTime);
		void setWorld(osg::ref_ptr<View> view, osg::ref_ptr<World> world);
    void setHud(osg::ref_ptr<View> view, osg::ref_ptr<Hud> hud);
    void setViewer(osg::ref_ptr<osgGaming::Viewer> viewer);
		void setGameSettings(osg::ref_ptr<GameSettings> settings);

		StateEvent* stateEvent_default();

    void setInjector(Injector& injector);

    static void toAbstractGameStateRefList(AbstractGameStateList& list, AbstractGameStateRefList& refList);

  private:
    template<typename TState>
    osg::ref_ptr<AbstractGameState> injectState()
    {
      osg::ref_ptr<TState> s = m_injector->inject<TState>();
      osg::ref_ptr<AbstractGameState> state = osg::ref_ptr<AbstractGameState>(dynamic_cast<AbstractGameState*>(s.get()));
      if (!state.valid())
      {
        assert(false && "State could not be injected.");
        return osg::ref_ptr<AbstractGameState>();
      }

      state->setInjector(*m_injector);
      return state;
    }

    virtual StateEvent* stateEvent_push(osg::ref_ptr<AbstractGameState> state);
    virtual StateEvent* stateEvent_replace(osg::ref_ptr<AbstractGameState> state);

	protected:
		virtual osg::ref_ptr<World> injectWorld(osgGaming::Injector& injector, osg::ref_ptr<View> view);
    virtual osg::ref_ptr<Hud> injectHud(osgGaming::Injector& injector, osg::ref_ptr<View> view);

    template<typename TState>
    StateEvent* stateEvent_push()
    {
      return stateEvent_push(injectState<TState>());
    }

    template<typename TState>
    StateEvent* stateEvent_replace()
    {
      return stateEvent_replace(injectState<TState>());
    }


		//StateEvent* stateEvent_push(AbstractGameStateList states);
	  virtual StateEvent* stateEvent_pop();
		//StateEvent* stateEvent_replace(AbstractGameStateList states);
	  virtual StateEvent* stateEvent_endGame();

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

    Injector* m_injector;

	};
}