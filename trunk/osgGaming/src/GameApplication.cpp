#include "GameApplication.h"
#include "Helper.h"
#include "GameException.h"

#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osg/LightModel>

using namespace osgGaming;
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

		if (!_gameSettings.valid())
		{
			setGameSettings(new GameSettings());
		}

		_updateStateCallback = new UpdateStateCallback(initialState, _world, _gameSettings);

		_world->getRootNode()->setUpdateCallback(_updateStateCallback);

		if (!_gameSettings->getFullscreenEnabled())
		{
			unsigned int screenWidth, screenHeight;

			GraphicsContext::getWindowingSystemInterface()->getScreenResolution(GraphicsContext::ScreenIdentifier(0), screenWidth, screenHeight);

			Vec2d windowResolution = _gameSettings->getWindowResolution();

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

		_viewer.realize();
		while (!_viewer.done() && !_updateStateCallback->gameEnded())
		{
			_viewer.frame();
		}

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