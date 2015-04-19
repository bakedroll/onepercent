#pragma once

#include <osgGaming/World.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/GameState.h>

#include <osg/NodeCallback>
#include <osgViewer/Viewer>

#include <vector>
#include <future>

namespace osgGaming
{
	typedef std::vector<osg::ref_ptr<GameState>> GameStateList;

	class GameApplication : public osg::NodeCallback
	{
	public:
		GameApplication();

		virtual void operator() (osg::Node* node, osg::NodeVisitor* nv);

		void setWorld(osg::ref_ptr<World> world);
		void setWorldLoading(osg::ref_ptr<World> world);
		void setGameSettings(osg::ref_ptr<GameSettings> settings);

		int run(osg::ref_ptr<GameState> initialState);

	private:
		double _lastSimulationTime;

		GameStateList _stateStack;

		osgViewer::Viewer _viewer;

		osg::ref_ptr<World> _world;
		osg::ref_ptr<World> _worldLoading;
		osg::ref_ptr<GameSettings> _gameSettings;

		bool _gameEnded;
		bool _isLoading;
		bool _resetTimeDiff;

		std::future<void> _loadingThreadFuture;
	};
}