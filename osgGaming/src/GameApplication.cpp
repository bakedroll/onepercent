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
{
}

void GameApplication::setWorld(ref_ptr<World> world)
{
	_viewer.setSceneData(world->getRootNode());

	if (_updateStateCallback.valid())
	{
		world->getRootNode()->setUpdateCallback(_updateStateCallback);
	}

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

		_updateStateCallback = new UpdateStateCallback(initialState, &_viewer, _world, _worldLoading, _gameSettings);

		_world->getRootNode()->setUpdateCallback(_updateStateCallback);

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