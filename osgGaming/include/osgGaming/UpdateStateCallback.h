#pragma once

#include "GameState.h"
#include "GameSettings.h"
#include "World.h"

#include <vector>

#include <osg/NodeCallback>

namespace osgGaming
{
	typedef std::vector<osg::ref_ptr<GameState>> GameStateList;

	class UpdateStateCallback : public osg::NodeCallback
	{
	public:
		UpdateStateCallback(
			osg::ref_ptr<GameState> initialState,
			osg::ref_ptr<World> world,
			osg::ref_ptr<GameSettings> settings);

		virtual void operator() (osg::Node* node, osg::NodeVisitor* nv);

		bool gameEnded();

	private:
		double _lastSimulationTime;

		GameStateList _stateStack;

		osg::ref_ptr<World> _world;
		osg::ref_ptr<GameSettings> _gameSettings;

		bool _gameEnded;
	};
}