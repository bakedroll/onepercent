#include "UpdateStateCallback.h"

#include <osg/NodeVisitor>

using namespace osgGaming;
using namespace osg;

UpdateStateCallback::UpdateStateCallback(
	osg::ref_ptr<GameState> initialState,
	osg::ref_ptr<World> world,
	osg::ref_ptr<GameSettings> settings)
		: NodeCallback(),
		  _world(world),
		  _gameSettings(settings),
		  _lastSimulationTime(0.0),
		  _gameEnded(false)
{
	_stateStack.push_back(initialState);
}

void UpdateStateCallback::operator() (Node* node, NodeVisitor* nv)
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

		StateEvent* e = (*it)->update(time_diff, _world, _gameSettings);

		if (e != NULL)
		{
			if (e->type == END_GAME)
			{
				_gameEnded = true;
			}

			delete e;
		}
	}
	else
	{
		_gameEnded = true;
	}
	
	traverse(node, nv);
}

bool UpdateStateCallback::gameEnded()
{
	return _gameEnded;
}