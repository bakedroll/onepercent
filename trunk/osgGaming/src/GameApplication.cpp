#include <osgGaming/GameApplication.h>
#include <osgGaming/Helper.h>
#include <osgGaming/GameException.h>

#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osg/LightModel>

using namespace osgGaming;
using namespace osgViewer;
using namespace osg;
using namespace std;

GameApplication::GameApplication()
	: NodeCallback(),
	  _lastSimulationTime(0.0),
	  _gameEnded(false),
	  _isLoading(false)
{

}

void GameApplication::operator() (Node* node, NodeVisitor* nv)
{
	double time = nv->getFrameStamp()->getSimulationTime();
	double time_diff = 0.0;

	if (_lastSimulationTime > 0.0)
	{
		time_diff = time - _lastSimulationTime;
	}

	_lastSimulationTime = time;

	if (!_stateStack.empty())
	{
		GameStateList::iterator it = _stateStack.end() - 1;

		if (!_isLoading && (*it)->isLoadingState())
		{
			_isLoading = true;

			_viewer.setSceneData(_worldLoading->getRootNode());
		}

		StateEvent* se;
		if (_isLoading)
		{
			se = (*it)->update(time_diff, _worldLoading, _gameSettings);
		}
		else
		{
			se = (*it)->update(time_diff, _world, _gameSettings);
		}

		if (se != NULL)
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

		initialState->initialize(_world, _gameSettings);
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