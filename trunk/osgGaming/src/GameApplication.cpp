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
	: NodeCallback(),
	  _lastSimulationTime(0.0),
	  _gameEnded(false),
	  _isLoading(false),
	  _resetTimeDiff(false)
{

}

void GameApplication::operator() (Node* node, NodeVisitor* nv)
{
	double time = nv->getFrameStamp()->getSimulationTime();
	double time_diff = 0.0;

	if (_resetTimeDiff == true)
	{
		_resetTimeDiff = false;
	}
	else if (_lastSimulationTime > 0.0)
	{
		time_diff = time - _lastSimulationTime;
	}

	_lastSimulationTime = time;

	if (!_stateStack.empty())
	{
		ref_ptr<GameState> state = *(_stateStack.end() - 1);

		state->setSimulationTime(time);
		state->setFrameTime(time_diff);

		if (!state->isInitialized())
		{
			state->setWorld(state->isLoadingState() ? _worldLoading : _world);
			state->setViewer(&_viewer);
			state->setGameSettings(_gameSettings);

			state->initialize(_inputManager->getResolutionWidth(), _inputManager->getResolutionHeight());
			state->setInitialized();

			if (state->isLoadingState())
			{
				if (!_isLoading)
				{
					_isLoading = true;

					attachWorld(_worldLoading);
				}

				ref_ptr<GameLoadingState> loadingState = static_cast<GameLoadingState*>(state.get());
				_loadingThreadFuture = async(launch::async, &GameLoadingState::loading_thread, loadingState, _world, _gameSettings);
			}
			else
			{
				if (_isLoading)
				{
					_isLoading = false;
					_resetTimeDiff = true;
					
					attachWorld(_world);
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

ref_ptr<GameSettings> GameApplication::getGameSettings()
{
	if (!_gameSettings.valid())
	{
		setGameSettings(new GameSettings());
	}

	return _gameSettings;
}

int GameApplication::run(ref_ptr<GameState> initialState)
{
	try
	{
		if (!_world.valid())
		{
			setWorld(new World());
		}

		if (!_worldLoading.valid())
		{
			setWorldLoading(new World());
		}

		if (!_gameSettings.valid())
		{
			setGameSettings(new GameSettings());
		}

		_stateStack.push_back(initialState);

		_inputManager = new InputManager();
		_inputManager->setCurrentWorld(_world);
		_inputManager->setCurrentState(initialState);

		unsigned int screenWidth, screenHeight;
		unsigned int width, height;
		GraphicsContext::getWindowingSystemInterface()->getScreenResolution(GraphicsContext::ScreenIdentifier(_gameSettings->getScreenNum()), screenWidth, screenHeight);

		if (!_gameSettings->getFullscreenEnabled())
		{
			Vec2i windowResolution = _gameSettings->getWindowResolution();

			_viewer.setUpViewInWindow(
				screenWidth / 2 - windowResolution.x() / 2,
				screenHeight / 2 - windowResolution.y() / 2,
				windowResolution.x(),
				windowResolution.y(),
				_gameSettings->getScreenNum());

			width = windowResolution.x();
			height = windowResolution.y();

			_inputManager->setViewer(&_viewer);
		}
		else
		{
			width = screenWidth;
			height = screenHeight;
		}

		_inputManager->updateResolution(width, height);

		attachWorld(_world);

		_viewer.addEventHandler(_inputManager);
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

void GameApplication::setWorld(osg::ref_ptr<World> world)
{
	_world = world;
}

void GameApplication::setWorldLoading(osg::ref_ptr<World> world)
{
	_worldLoading = world;
}

void GameApplication::setGameSettings(osg::ref_ptr<GameSettings> settings)
{
	settings->load();

	_gameSettings = settings;
}

void GameApplication::attachWorld(osg::ref_ptr<World> world)
{
	_viewer.setSceneData(world->getRootNode());
	_viewer.setCameraManipulator(world->getCameraManipulator());

	_inputManager->setCurrentWorld(world);
	_inputManager->updateResolution();
}

void GameApplication::popState()
{
	_stateStack.pop_back();

	if (_stateStack.size() == 0)
	{
		return;
	}
	
	_inputManager->setCurrentState(*(_stateStack.end() - 1));
}

void GameApplication::pushState(osg::ref_ptr<GameState> state)
{
	_stateStack.push_back(state);
	_inputManager->setCurrentState(state);
}

void GameApplication::replaceState(osg::ref_ptr<GameState> state)
{
	_stateStack.pop_back();
	pushState(state);
}