#pragma once

#include <osgGaming/Injector.h>
#include <osgGaming/InjectionContainer.h>
#include <osgGaming/SimulationCallback.h>
#include <osgGaming/GameStateStack.h>

#include "Viewer.h"
#include "TimerFactory.h"

namespace osgGaming
{
  class Signal;
  class World;
  class Hud;
  class InputManager;
  class AbstractGameState;
  class GameSettings;
  class View;

  class GameApplication : public SimulationCallback
	{
  protected:
    virtual void initialize(Injector& injector);
    virtual void deinitialize();

    virtual int run(osg::ref_ptr<AbstractGameState>& initialState);
    virtual int run(AbstractGameState::AbstractGameStateRefList initialStates);

  public:
    template<typename TState>
    int run()
    {
      registerComponents(m_container);
      Injector injector(m_container);
      m_injector = &injector;

      m_container.registerSingletonType<TimerFactory>();
      m_container.registerType<World>();
      m_container.registerType<Hud>();

      osg::ref_ptr<TState> s = m_injector->inject<TState>();
      osg::ref_ptr<AbstractGameState> state = osg::ref_ptr<AbstractGameState>(dynamic_cast<AbstractGameState*>(s.get()));
      s.release();

      if (!state.valid())
      {
        assert(false && "TState has to be an AbstractGameState.");
        return -1;
      }

      initialize(*m_injector);

      state->setInjector(*m_injector);
      return run(state);
    }

    GameApplication();
    ~GameApplication();

    virtual void action(osg::Node* node, osg::NodeVisitor* nv, double simTime, double timeDiff) override;

    osg::ref_ptr<World> getDefaultWorld();
    osg::ref_ptr<Hud> getDefaultHud();
    osg::ref_ptr<GameSettings> getDefaultGameSettings();

    void setDefaultWorld(osg::ref_ptr<World> world);
    void setDefaultHud(osg::ref_ptr<Hud> hud);
    void setDefaultGameSettings(osg::ref_ptr<GameSettings> settings);

    osgGaming::Signal& onEndGameSignal();
    osg::ref_ptr<osgGaming::Viewer> getViewer();

  protected:
    virtual void registerComponents(InjectionContainer& container);
    void registerEssentialComponents();

    virtual int mainloop() = 0;
    virtual osg::ref_ptr<InputManager> createInputManager(osg::ref_ptr<osgGaming::View> view) = 0;

    virtual void newStateEvent(osg::ref_ptr<AbstractGameState> state);
    virtual void stateAttachedEvent(osg::ref_ptr<AbstractGameState> state);

    bool isGameEnded() const;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

    InjectionContainer m_container;
    Injector* m_injector;

	};
}
