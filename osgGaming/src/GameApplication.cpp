#include <osgGaming/GameApplication.h>
#include <osgGaming/GameException.h>
#include <osgGaming/GameLoadingState.h>
#include <osgGaming/TransformableCameraManipulator.h>
#include <osgGaming/TimerFactory.h>

#include <chrono>
#include <future>
#include <iostream>
#include <osgGaming/NativeInputManager.h>
#include <osgGaming/Observable.h>

using namespace osg;
using namespace std;

namespace osgGaming
{

  struct GameApplication::Impl
  {
    Impl(GameApplication* app, osg::ref_ptr<osgGaming::View> v)
      : base(app)
      , viewer(new osgGaming::Viewer())
      , view(v)
      , gameEnded(false)
      , isLoading(false)
    {
      if (!view)
        view = new osgGaming::NativeView();

      viewer->addView(view);
    }

    void initializeState(osg::ref_ptr<AbstractGameState> state)
    {
      prepareStateWorldAndHud(state);

      state->setViewer(viewer);
      state->setGameSettings(base->getDefaultGameSettings());

      state->initialize();
    }

    void prepareStateWorldAndHud(osg::ref_ptr<AbstractGameState> state)
    {
      if (!state->isWorldAndHudPrepared())
      {
        state->prepareWorldAndHud();

        if (!state->getWorld().valid())
        {
          state->setWorld(base->getDefaultWorld());
        }

        if (!state->getHud().valid())
        {
          state->setHud(base->getDefaultHud());
        }
      }
    }

    void prepareStateWorldAndHud(AbstractGameState::AbstractGameStateList states)
    {
      for (AbstractGameState::AbstractGameStateList::iterator it = states.begin(); it != states.end(); ++it)
      {
        prepareStateWorldAndHud(*it);
      }
    }

    osg::ref_ptr<InputManager> obtainInputManager(osg::ref_ptr<osgGaming::View> view)
    {
      InputManager::Ptr im;
      InputManagerMap::iterator it = inputManagers.find(view);
      if (it == inputManagers.end())
        im = base->createInputManager(view);
      else
        im = it->second;

      return im;
    }

    void attachState(osg::ref_ptr<AbstractGameState> state)
    {
      if (!state->isLoadingState())
      {
        base->setDefaultWorld(state->getWorld());
        base->setDefaultHud(state->getHud());
      }

      if (!inputManager.valid())
      {
        inputManager = obtainInputManager(view);
        inputManager->setGameStateStack(&gameStateStack);
        inputManager->setView(view);
        inputManager->setIsInizialized(true);
      }

      osg::ref_ptr<TransformableCameraManipulator> manipulator = state->getWorld()->getCameraManipulator();
      manipulator->setCamera(view->getSceneCamera());

      view->setSceneData(state->getWorld()->getRootNode());
      view->setHud(state->getHud());
      view->setCameraManipulator(state->getWorld()->getCameraManipulator());

      inputManager->updateNewRunningStates();
      //inputManager->setCurrentState(state);
    }

    typedef std::map<osgGaming::View::Ptr, osgGaming::InputManager::Ptr> InputManagerMap;

    GameApplication* base;
    GameStateStack gameStateStack;

    osg::ref_ptr<osgGaming::Viewer> viewer;
    osg::ref_ptr<osgGaming::View> view;

    osg::ref_ptr<InputManager> inputManager;

    osg::ref_ptr<World> defaultWorld;
    osg::ref_ptr<Hud> defaultHud;
    osg::ref_ptr<GameSettings> defaultGameSettings;

    bool gameEnded;
    bool isLoading;

    std::future<void> loadingThreadFuture;

    osgGaming::Signal onEndGameSignal;

    InputManagerMap inputManagers;
  };

  GameApplication::GameApplication(osg::ref_ptr<osgGaming::View> view)
    : SimulationCallback()
    , m(new Impl(this, view))
  {
  }

  GameApplication::~GameApplication()
  {
  }

  void GameApplication::action(Node* node, NodeVisitor* nv, double simTime, double timeDiff)
  {
    if (m->gameEnded)
    {
      m->onEndGameSignal.trigger();
      return;
    }

    TimerFactory::getInstance()->updateRegisteredTimers(simTime);

    if (!m->gameStateStack.isEmpty())
    {
      bool attach = m->gameStateStack.attachRequired();

      m->gameStateStack.begin(AbstractGameState::ALL, false);
      while (m->gameStateStack.next())
      {
        ref_ptr<AbstractGameState> state = m->gameStateStack.get();

        state->setSimulationTime(simTime);
        state->setFrameTime(timeDiff);

        bool initialized = state->isInitialized();

        if (!initialized)
        {
          m->initializeState(state);
          state->setInitialized();
        }

        if (attach && m->gameStateStack.isTop())
        {
          m->attachState(state);
        }

        if (!initialized)
        {
          if (state->isLoadingState())
          {
            if (!m->isLoading)
            {
              m->isLoading = true;
            }

            ref_ptr<GameLoadingState> loadingState = static_cast<GameLoadingState*>(state.get());

            AbstractGameState::AbstractGameStateList nextStates = loadingState->getNextStates();
            m->prepareStateWorldAndHud(nextStates);

            ref_ptr<AbstractGameState> nextState = *nextStates.begin();

            m->loadingThreadFuture = async(launch::async,
              &GameLoadingState::loading_thread,
              loadingState,
              nextState->getWorld(),
              nextState->getHud(),
              getDefaultGameSettings());

          }
          else
          {
            if (m->isLoading)
            {
              m->isLoading = false;
              resetTimeDiff();
            }
          }
        }

        // update state
        GameState::StateEvent* se;

        if (m->gameStateStack.hasBehavior(state, AbstractGameState::UPDATE))
        {
          se = state->update();
        }
        else
        {
          se = state->stateEvent_default();
        }

        if (m->isLoading)
        {
          if (m->loadingThreadFuture.wait_for(chrono::milliseconds(0)) == future_status::ready)
          {
            try
            {
              m->loadingThreadFuture.get();
            }
            catch (GameException& e)
            {
              throw e;
            }
            catch (std::exception& e)
            {
              throw e;
            }

            ref_ptr<GameLoadingState> loadingState = static_cast<GameLoadingState*>(state.get());

            m->gameStateStack.replaceState(loadingState->getNextStates());

            break;
          }
        }
        else if (se != nullptr)
        {
          switch (se->type)
          {
          case GameState::POP:
            m->gameStateStack.popState();
            break;
          case GameState::PUSH:
            m->gameStateStack.pushStates(se->referencedStates);
            break;
          case GameState::REPLACE:
            m->gameStateStack.replaceState(se->referencedStates);
            break;
          case GameState::END_GAME:
            m->gameEnded = true;
            break;
          }

          delete se;

          break;
        }

      }
    }
    else
    {
      m->gameEnded = true;
    }

    traverse(node, nv);
  }

  ref_ptr<World> GameApplication::getDefaultWorld()
  {
    if (!m->defaultWorld.valid())
    {
      setDefaultWorld(new World());
    }

    return m->defaultWorld;
  }

  ref_ptr<Hud> GameApplication::getDefaultHud()
  {
    if (!m->defaultHud.valid())
    {
      setDefaultHud(new Hud());
    }

    return m->defaultHud;
  }

  ref_ptr<GameSettings> GameApplication::getDefaultGameSettings()
  {
    if (!m->defaultGameSettings.valid())
    {
      setDefaultGameSettings(new GameSettings());
    }

    return m->defaultGameSettings;
  }

  void GameApplication::setDefaultWorld(ref_ptr<World> world)
  {
    m->defaultWorld = world;
  }

  void GameApplication::setDefaultHud(ref_ptr<Hud> hud)
  {
    m->defaultHud = hud;
  }

  void GameApplication::setDefaultGameSettings(ref_ptr<GameSettings> settings)
  {
    settings->load();

    m->defaultGameSettings = settings;
  }

  int GameApplication::run(ref_ptr<AbstractGameState> initialState)
  {
    GameStateStack::AbstractGameStateList states;
    states.push_back(initialState);

    return run(states);
  }

  int GameApplication::run(GameStateStack::AbstractGameStateList initialStates)
  {
    try
    {
      m->gameStateStack.pushStates(initialStates);
      m->view->getRootGroup()->setUpdateCallback(this);

      return mainloop();
    }
    catch (GameException& e)
    {
      printf("Exception: %s\n", e.getMessage().c_str());

      std::cin.ignore();
    }
    catch (exception& e)
    {
      printf("Exception: %s\n", e.what());

      std::cin.ignore();
    }

    return -1;
  }

  osgGaming::Signal& GameApplication::onEndGameSignal()
  {
    return m->onEndGameSignal;
  }

  osg::ref_ptr<osgGaming::Viewer> GameApplication::getViewer()
  {
    return m->viewer;
  }

  bool GameApplication::isGameEnded() const
  {
    return m->gameEnded;
  }

}