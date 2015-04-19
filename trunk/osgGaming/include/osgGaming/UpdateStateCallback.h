#pragma once

#include <osgGaming/GameState.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/World.h>

#include <vector>

#include <osg/NodeCallback>
#include <osgViewer/Viewer>

namespace osgGaming
{
	typedef std::vector<osg::ref_ptr<GameState>> GameStateList;

	class UpdateStateCallback : public osg::NodeCallback
	{
	public:
		UpdateStateCallback(
			osg::ref_ptr<GameState> initialState,
			osg::ref_ptr<osgViewer::Viewer> viewer,
			osg::ref_ptr<World> world,
			osg::ref_ptr<World> worldLoading,
			osg::ref_ptr<GameSettings> settings);

		virtual void operator() (osg::Node* node, osg::NodeVisitor* nv);

		bool gameEnded();

	private:
		void popState();
		void pushState(osg::ref_ptr<GameState> state);

		double _lastSimulationTime;

		GameStateList _stateStack;

		osg::ref_ptr<osgViewer::Viewer> _viewer;

		osg::ref_ptr<World> _world;
		osg::ref_ptr<World> _worldLoading;
		osg::ref_ptr<GameSettings> _gameSettings;

		bool _gameEnded;
		bool _isLoading;
	};
}