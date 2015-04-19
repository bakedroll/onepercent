#include <osgGaming/GameApplication.h>
#include <osgGaming/Helper.h>
#include <osgGaming/GameException.h>
#include <osgGaming/GameLoadingState.h>

#include <osgViewer/Viewer>
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

		ref_ptr<World> currentWorld = state->isLoadingState() ? _worldLoading : _world;

		if (!state->isInitialized())
		{
			state->initialize(currentWorld, _gameSettings);
			state->setInitialized();

			if (state->isLoadingState())
			{
				if (!_isLoading)
				{
					_isLoading = true;
					_viewer.setSceneData(_worldLoading->getRootNode());
				}

				ref_ptr<GameLoadingState> loadingState = static_cast<GameLoadingState*>(state.get());
				_loadingThreadFuture = async(launch::async, &GameLoadingState::load, loadingState, _world, _gameSettings);
			}
			else
			{
				if (_isLoading)
				{
					_isLoading = false;
					_viewer.setSceneData(_world->getRootNode());
					_resetTimeDiff = true;
				}
			}
		}

		// Update state
		StateEvent* se = state->update(time_diff, currentWorld, _gameSettings);

		if (_isLoading)
		{
			if (_loadingThreadFuture.wait_for(chrono::milliseconds(0)) == future_status::ready)
			{
				ref_ptr<GameLoadingState> loadingState = static_cast<GameLoadingState*>(state.get());

				_stateStack.pop_back();
				_stateStack.push_back(loadingState->getNextState());
			}
		}
		else if (se != NULL)
		{
			switch (se->type)
			{
			case POP:
				_stateStack.pop_back();
				break;
			case PUSH:
				_stateStack.push_back(se->referencedState);
				break;
			case REPLACE:
				_stateStack.pop_back();
				_stateStack.push_back(se->referencedState);
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

void GameApplication::setWorld(ref_ptr<World> world)
{
	_viewer.setSceneData(world->getRootNode());

	world->getRootNode()->setUpdateCallback(this);

	_world = world;
}

void GameApplication::setWorldLoading(osg::ref_ptr<World> world)
{
	world->getRootNode()->setUpdateCallback(this);

	_worldLoading = world;
}

void GameApplication::setGameSettings(osg::ref_ptr<GameSettings> settings)
{
	settings->load();

	_gameSettings = settings;
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

		if (!_gameSettings->getFullscreenEnabled())
		{
			unsigned int screenWidth, screenHeight;

			GraphicsContext::getWindowingSystemInterface()->getScreenResolution(GraphicsContext::ScreenIdentifier(0), screenWidth, screenHeight);

			Vec2i windowResolution = _gameSettings->getWindowResolution();

			_viewer.setUpViewInWindow(
				screenWidth / 2 - windowResolution.x() / 2,
				screenHeight / 2 - windowResolution.y() / 2,
				windowResolution.x(),
				windowResolution.y());
		}

		// TODO: Camera
		_viewer.getCamera()->setClearColor(Vec4(0.0, 0.0, 0.0, 1.0));

		/*
		ref_ptr<Follower> follower = new Follower();
		viewer.setCamera(follower);

		follower->setPosition(Vec3(0, -4, 0));
		follower->updateLookAtMatrix(); */

		/*_viewer.realize();
		while (!_viewer.done() && !_updateStateCallback->gameEnded())
		{
			_viewer.frame();
		}*/

		_viewer.run();

		return 0;
	}
	catch (GameException& e)
	{
		printf("Exception: %s\n", e.getMessage());
	}
	catch (exception& e)
	{
		printf("Exception: %s\n", e.what());
	}

	return -1;
}