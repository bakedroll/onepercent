#pragma once

#include "osgGaming/Viewer.h"

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
		enum class StateEventType
		{
			Push,
			Pop,
			Replace,
			EndGame
		};

    enum class StateProperties
    {
      UpdateTop       = 1,
      UpdateAlways    = 2,
      GuiEventsTop    = 4,
      GuiEventsAlways = 8
    };

    enum class StateBehavior
		{
			Update,
			GuiEvent,
			All
		};

    using AbstractGameStateList    = std::vector<osg::ref_ptr<AbstractGameState>>;
    using AbstractGameStateRefList = std::vector<std::reference_wrapper<osg::ref_ptr<AbstractGameState>>>;

    struct StateEvent
    {
      StateEventType        type;
      AbstractGameStateList referencedStates;
    };

    AbstractGameState();
	  virtual ~AbstractGameState();

		bool isInitialized() const;
		bool isWorldAndHudPrepared() const;
    void setWorldAndHudPrepared();
		bool isFirstUpdate() const;
		void setInitialized();

		void setDirty(StateBehavior behavior);
		bool isDirty(StateBehavior behavior) const;

		virtual void initialize();
		virtual StateEvent* update();

		virtual bool isLoadingState() const = 0;
		virtual unsigned char getProperties() const;

		virtual void onKeyPressedEvent(int key);
		virtual void onKeyReleasedEvent(int key);

		virtual void onMousePressedEvent(int button, float x, float y);
		virtual void onMouseReleasedEvent(int button, float x, float y);

		virtual void onMouseMoveEvent(float x, float y);

		virtual void onScrollEvent(osgGA::GUIEventAdapter::ScrollingMotion motion);

		virtual void onDragEvent(int button, const osg::Vec2f& origin, const osg::Vec2f& position, const osg::Vec2f& change);
		virtual void onDragBeginEvent(int button, const osg::Vec2f& origin);
		virtual void onDragEndEvent(int button, const osg::Vec2f& origin, const osg::Vec2f& position);

		virtual void onResizeEvent(float width, float height);

    void prepareWorldAndHud(Injector& injector, const osg::ref_ptr<View>& view);

    double                     getSimulationTime() const;
    double                     getFrameTime() const;
    osg::ref_ptr<World>        getWorld(const osg::ref_ptr<View>& view) const;
    osg::ref_ptr<Hud>          getHud(const osg::ref_ptr<View>& view) const;
    osg::ref_ptr<Viewer>       getViewer() const;
    osg::ref_ptr<View>         getView(int i) const;
    osg::ref_ptr<GameSettings> getGameSettings() const;

    void setSimulationTime(double simulationTime);
		void setFrameTime(double frameTime);
		void setWorld(const osg::ref_ptr<View>& view, const osg::ref_ptr<World>& world);
    void setHud(const osg::ref_ptr<View>& view, const osg::ref_ptr<Hud>& hud);
    void setViewer(const osg::ref_ptr<Viewer>& viewer);
		void setGameSettings(const osg::ref_ptr<GameSettings>& settings);

		StateEvent* stateEvent_default();

    void setInjector(Injector& injector);

    static void toAbstractGameStateRefList(AbstractGameStateList& list, AbstractGameStateRefList& refList);

  private:
    template<typename TState>
    osg::ref_ptr<AbstractGameState> injectState()
    {
      auto s     = m_injector->inject<TState>();
      auto state = osg::ref_ptr<AbstractGameState>(dynamic_cast<AbstractGameState*>(s.get()));
      if (!state.valid())
      {
        assert(false && "State could not be injected.");
        return osg::ref_ptr<AbstractGameState>();
      }

      state->setInjector(*m_injector);
      return state;
    }

    virtual StateEvent* stateEvent_push(const osg::ref_ptr<AbstractGameState>& state);
    virtual StateEvent* stateEvent_replace(const osg::ref_ptr<AbstractGameState>& state);

	protected:
    virtual osg::ref_ptr<World> injectWorld(Injector& injector, const osg::ref_ptr<View>& view);
    virtual osg::ref_ptr<Hud>   injectHud(Injector& injector, const osg::ref_ptr<View>& view);

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

	  virtual StateEvent* stateEvent_pop();
	  virtual StateEvent* stateEvent_endGame();

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

    Injector* m_injector;

	};
}