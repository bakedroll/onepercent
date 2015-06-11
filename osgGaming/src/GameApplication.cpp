#include <osgGaming/GameApplication.h>
#include <osgGaming/Helper.h>
#include <osgGaming/GameException.h>
#include <osgGaming/GameLoadingState.h>
#include <osgGaming/TransformableCameraManipulator.h>

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
	  _isLoading(false),
	  _attachState(true)
{

}

void GameApplication::action(Node* node, NodeVisitor* nv, double simTime, double timeDiff)
{
	if (!_stateStack.empty())
	{
		ref_ptr<AbstractGameState> state = *(--_stateStack.end());

		state->setSimulationTime(simTime);
		state->setFrameTime(timeDiff);

		bool initialized = state->isInitialized();

		if (!initialized)
		{
			initializeState(state);
			state->setInitialized();
		}

		if (_attachState)
		{
			attachState(state);

			_attachState = false;
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

				ref_ptr<GameState> nextState = loadingState->getNextState();
				prepareStateWorldAndHud(nextState);

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
		

		// Update state
		StateEvent* se = state->update();

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

				replaceState(loadingState->getNextState());
			}
		}
		else if (se != NULL)
		{
			switch (se->type)
			{
			case POP:
				popState();
				break;
			case PUSH:
				pushState(se->referencedState);
				break;
			case REPLACE:
				replaceState(se->referencedState);
				break;
			case END_GAME:
				_gameEnded = true;
				break;
			}

			delete se;
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
	try
	{
		_stateStack.push_back(initialState);

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
		_inputManager->setViewer(&_viewer);

		_viewer.addEventHandler(_inputManager);
	}

	_viewer.setSceneData(state->getWorld()->getRootNode());
	_viewer.setHud(state->getHud());
	_viewer.setCameraManipulator(state->getWorld()->getCameraManipulator());

	_inputManager->setCurrentState(state);
}

void GameApplication::popState()
{
	_stateStack.pop_back();

	if (_stateStack.size() == 0)
	{
		return;
	}
	
	_attachState = true;
}

void GameApplication::pushState(ref_ptr<AbstractGameState> state)
{
	_stateStack.push_back(state);

	_attachState = true;
}

void GameApplication::replaceState(ref_ptr<AbstractGameState> state)
{
	_stateStack.pop_back();
	pushState(state);
}