#include <osgGaming/GameApplication.h>
#include <osgGaming/GameException.h>
#include <osgGaming/GameLoadingState.h>
#include <osgGaming/TransformableCameraManipulator.h>
#include <osgGaming/TimerFactory.h>

#include <chrono>
#include <iostream>

using namespace osg;
using namespace std;

namespace osgGaming
{

GameApplication::GameApplication(osg::ref_ptr<osgGaming::View> view)
  : SimulationCallback()
  , m_viewer(new osgGaming::Viewer())
  , m_view(view)
  , m_gameEnded(false)
  , m_isLoading(false)
{
  if (!m_view)
    m_view = new osgGaming::View();

  m_viewer->addView(m_view);
}

void GameApplication::action(Node* node, NodeVisitor* nv, double simTime, double timeDiff)
{
  TimerFactory::getInstance()->updateRegisteredTimers(simTime);

  if (!m_gameStateStack.isEmpty())
  {
    bool attach = m_gameStateStack.attachRequired();

    m_gameStateStack.begin(AbstractGameState::ALL, false);
    while (m_gameStateStack.next())
    {
      ref_ptr<AbstractGameState> state = m_gameStateStack.get();

      state->setSimulationTime(simTime);
      state->setFrameTime(timeDiff);

      bool initialized = state->isInitialized();

      if (!initialized)
      {
        initializeState(state);
        state->setInitialized();
      }

      if (attach && m_gameStateStack.isTop())
      {
        attachState(state);
      }

      if (!initialized)
      {
        if (state->isLoadingState())
        {
          if (!m_isLoading)
          {
            m_isLoading = true;
          }

          ref_ptr<GameLoadingState> loadingState = static_cast<GameLoadingState*>(state.get());

          AbstractGameState::AbstractGameStateList nextStates = loadingState->getNextStates();
          prepareStateWorldAndHud(nextStates);

          ref_ptr<AbstractGameState> nextState = *nextStates.begin();

          m_loadingThreadFuture = async(launch::async,
            &GameLoadingState::loading_thread,
            loadingState,
            nextState->getWorld(),
            nextState->getHud(),
            getDefaultGameSettings());

        }
        else
        {
          if (m_isLoading)
          {
            m_isLoading = false;
            resetTimeDiff();
          }
        }
      }

      // update state
      GameState::StateEvent* se;

      if (m_gameStateStack.hasBehavior(state, AbstractGameState::UPDATE))
      {
        se = state->update();
      }
      else
      {
        se = state->stateEvent_default();
      }

      if (m_isLoading)
      {
        if (m_loadingThreadFuture.wait_for(chrono::milliseconds(0)) == future_status::ready)
        {
          try
          {
            m_loadingThreadFuture.get();
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

          m_gameStateStack.replaceState(loadingState->getNextStates());

          break;
        }
      }
      else if (se != nullptr)
      {
        switch (se->type)
        {
        case GameState::POP:
          m_gameStateStack.popState();
          break;
        case GameState::PUSH:
          m_gameStateStack.pushStates(se->referencedStates);
          break;
        case GameState::REPLACE:
          m_gameStateStack.replaceState(se->referencedStates);
          break;
        case GameState::END_GAME:
          m_gameEnded = true;
          break;
        }

        delete se;

        break;
      }

    }
  }
  else
  {
    m_gameEnded = true;
  }

  traverse(node, nv);
}

ref_ptr<World> GameApplication::getDefaultWorld()
{
  if (!m_defaultWorld.valid())
  {
    setDefaultWorld(new World());
  }

  return m_defaultWorld;
}

ref_ptr<Hud> GameApplication::getDefaultHud()
{
  if (!m_defaultHud.valid())
  {
    setDefaultHud(new Hud());
  }

  return m_defaultHud;
}

ref_ptr<GameSettings> GameApplication::getDefaultGameSettings()
{
  if (!m_defaultGameSettings.valid())
  {
    setDefaultGameSettings(new GameSettings());
  }

  return m_defaultGameSettings;
}

void GameApplication::setDefaultWorld(ref_ptr<World> world)
{
  m_defaultWorld = world;
}

void GameApplication::setDefaultHud(ref_ptr<Hud> hud)
{
  m_defaultHud = hud;
}

void GameApplication::setDefaultGameSettings(ref_ptr<GameSettings> settings)
{
  settings->load();

  m_defaultGameSettings = settings;
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
    m_gameStateStack.pushStates(initialStates);
    m_view->getRootGroup()->setUpdateCallback(this);

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

osg::ref_ptr<osgGaming::Viewer> GameApplication::getViewer()
{
  return m_viewer;
}

osg::ref_ptr<osgGaming::View> GameApplication::getView()
{
  return m_view;
}

int GameApplication::mainloop()
{
  ref_ptr<GameSettings> settings = getDefaultGameSettings();

  m_viewer->setFullscreenEnabled(0, settings->getFullscreenEnabled());
  m_viewer->setWindowedResolution(0, settings->getWindowedResolution());
  m_view->setScreenNum(settings->getScreenNum());

  m_view->setupResolution();

  m_viewer->setKeyEventSetsDone(0);

  m_viewer->realize();

  while (isGameRunning())
  {
    m_viewer->frame();
  }

  return 0;
}

bool GameApplication::isGameRunning() const
{
  return !m_viewer->done() && !m_gameEnded;
}

void GameApplication::initializeState(ref_ptr<AbstractGameState> state)
{
  prepareStateWorldAndHud(state);

  state->setViewer(m_viewer);
  state->setGameSettings(getDefaultGameSettings());

  state->initialize();
}

void GameApplication::prepareStateWorldAndHud(ref_ptr<AbstractGameState> state)
{
  if (!state->isWorldAndHudPrepared())
  {
    state->prepareWorldAndHud();

    if (!state->getWorld().valid())
    {
      state->setWorld(getDefaultWorld());
    }

    if (!state->getHud().valid())
    {
      state->setHud(getDefaultHud());
    }
  }
}

void GameApplication::prepareStateWorldAndHud(AbstractGameState::AbstractGameStateList states)
{
  for (AbstractGameState::AbstractGameStateList::iterator it = states.begin(); it != states.end(); ++it)
  {
    prepareStateWorldAndHud(*it);
  }
}

void GameApplication::attachState(ref_ptr<AbstractGameState> state)
{
  if (!state->isLoadingState())
  {
    setDefaultWorld(state->getWorld());
    setDefaultHud(state->getHud());
  }

  if (!m_inputManager.valid())
  {
    m_inputManager = new InputManager();
    m_inputManager->setGameStateStack(&m_gameStateStack);
    m_inputManager->setView(m_view);

    m_view->addEventHandler(m_inputManager);
  }

  m_view->setSceneData(state->getWorld()->getRootNode());
  m_view->setHud(state->getHud());
  m_view->setCameraManipulator(state->getWorld()->getCameraManipulator());

  m_inputManager->updateNewRunningStates();
  //m_inputManager->setCurrentState(state);
}

}