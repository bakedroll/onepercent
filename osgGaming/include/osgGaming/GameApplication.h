#pragma once

#include "World.h"
#include "GameSettings.h"
#include "UpdateStateCallback.h"
#include "GameState.h"

#include <osgViewer/Viewer>

namespace osgGaming
{
	class GameApplication
	{
	public:
		GameApplication();

		void setWorld(osg::ref_ptr<World> world);
		void setGameSettings(osg::ref_ptr<GameSettings> settings);

		int run(osg::ref_ptr<GameState> initialState);

	private:
		osgViewer::Viewer _viewer;

		osg::ref_ptr<World> _world;
		osg::ref_ptr<GameSettings> _gameSettings;
		osg::ref_ptr<UpdateStateCallback> _updateStateCallback;
	};
}