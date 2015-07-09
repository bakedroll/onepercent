#include <osgGaming/GameApplication.h>
#include <osgGaming/Helper.h>
#include <osgGaming/GameException.h>
#include <osgGaming/GameLoadingState.h>
#include <osgGaming/TransformableCameraManipulator.h>
#include <osgGaming/TimerFactory.h>

#include <osg/PositionAttitudeTransform>
#include <osg/LightModel>

#include <thread>
#include <chrono>

using namespace osgGaming;
using namespace osgViewer;
using namespace osg;
using namespace std;

GameApplication::GameApplication()
	: SimulationCallback(),
	  _gameEnded(false),
	  _isLoading(false)
{

}

void GameApplication::action(Node* node, NodeVisitor* nv, double simTime, double timeDiff)
{
	TimerFactory::getInstance()->updateRegisteredTimers(simTime);

	if (!_gameStateStack.isEmpty())
	{
		bool attach = _gameStateStack.attachRequired();

		_gameStateStack.begin(AbstractGameState::ALL, false);
		while (_gameStateStack.next())
		{
			ref_ptr<AbstractGameState> state = _gameStateStack.get();

			state->setSimulationTime(simTime);
			state->setFrameTime(timeDiff);

			bool initialized = state->isInitialized();

			if (!initialized)
			{
				initializeState(state);
				state->setInitialized();
			}

			if (attach && _gameStateStack.isTop())
			{
				attachState(state);
			}

			if (!initialized)
			{
				if (state->isLoadingState())
				{
					if (!_isLoading)
					{
						_isLoading = true;
					}

					ref_ptr<GameLoadingState> loadingState = static_cast<GameLoadingState*>(state.get());

					AbstractGameState::AbstractGameStateList nextStates = loadingState->getNextStates();
					prepareStateWorldAndHud(nextStates);

					ref_ptr<AbstractGameState> nextState = *nextStates.begin();

					_loadingThreadFuture = async(launch::async,
						&GameLoadingState::loading_thread,
						loadingState,
						nextState->getWorld(),
						nextState->getHud(),
						getDefaultGameSettings());

				}
				else
				{
					if (_isLoading)
					{
						_isLoading = false;
						resetTimeDiff();
					}
				}
			}

			// update state
			GameState::StateEvent* se;
			
			if (_gameStateStack.hasBehavior(state, AbstractGameState::UPDATE))
			{
				se = state->update();
			}
			else
			{
				se = state->stateEvent_default();
			}

			if (_isLoading)
			{
				if (_loadingThreadFuture.wait_for(chrono::milliseconds(0)) == future_status::ready)
				{
					try
					{
						_loadingThreadFuture.get();
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

					_gameStateStack.replaceState(loadingState->getNextStates());

					break;
				}
			}
			else if (se != NULL)
			{
				switch (se->type)
				{
				case GameState::POP:
					_gameStateStack.popState();
					break;
				case GameState::PUSH:
					_gameStateStack.pushStates(se->referencedStates);
					break;
				case GameState::REPLACE:
					_gameStateStack.replaceState(se->referencedStates);
					break;
				case GameState::END_GAME:
					_gameEnded = true;
					break;
				}

				delete se;

				break;
			}

		}
	}
	else
	{
		_gameEnded = true;
	}

	traverse(node, nv);
}

ref_ptr<World> GameApplication::getDefaultWorld()
{
	if (!_defaultWorld.valid())
	{
		setDefaultWorld(new World());
	}

	return _defaultWorld;
}

ref_ptr<Hud> GameApplication::getDefaultHud()
{
	if (!_defaultHud.valid())
	{
		setDefaultHud(new Hud());
	}

	return _defaultHud;
}

ref_ptr<GameSettings> GameApplication::getDefaultGameSettings()
{
	if (!_defaultGameSettings.valid())
	{
		setDefaultGameSettings(new GameSettings());
	}

	return _defaultGameSettings;
}

void GameApplication::setDefaultWorld(ref_ptr<World> world)
{
	_defaultWorld = world;
}

void GameApplication::setDefaultHud(ref_ptr<Hud> hud)
{
	_defaultHud = hud;
}

void GameApplication::setDefaultGameSettings(ref_ptr<GameSettings> settings)
{
	settings->load();

	_defaultGameSettings = settings;
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
		_gameStateStack.pushStates(initialStates);

		ref_ptr<GameSettings> settings = getDefaultGameSettings();

		_viewer.setFullscreenEnabled(settings->getFullscreenEnabled());
		_viewer.setWindowedResolution(settings->getWindowedResolution());
		_viewer.setScreenNum(settings->getScreenNum());

		_viewer.setupResolution();

		_viewer.setKeyEventSetsDone(0);
		_viewer.getRootGroup()->setUpdateCallback(this);

		_viewer.realize();
		while (!_viewer.done() && !_gameEnded)
		{
			_viewer.frame();
		}

		return 0;
	}
	catch (GameException& e)
	{
		printf("Exception: %s\n", e.getMessage().data());
	}
	catch (exception& e)
	{
		printf("Exception: %s\n", e.what());
	}

	return -1;
}

void GameApplication::initializeState(ref_ptr<AbstractGameState> state)
{
	prepareStateWorldAndHud(state);

	state->setViewer(&_viewer);
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

	if (!_inputManager.valid())
	{
		_inputManager = new InputManager();
		_inputManager->setGameStateStack(&_gameStateStack);
		_inputManager->setViewer(&_viewer);

		_viewer.addEventHandler(_inputManager);
	}

	_viewer.setSceneData(state->getWorld()->getRootNode());
	_viewer.setHud(state->getHud());
	_viewer.setCameraManipulator(state->getWorld()->getCameraManipulator());

	_inputManager->updateNewRunningStates();
	//_inputManager->setCurrentState(state);
}