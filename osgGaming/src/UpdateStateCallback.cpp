#include <osgGaming/UpdateStateCallback.h>

#include <osg/NodeVisitor>

using namespace osgViewer;
using namespace osgGaming;
using namespace osg;

UpdateStateCallback::UpdateStateCallback(
	osg::ref_ptr<GameState> initialState,
	osg::ref_ptr<Viewer> viewer,
	osg::ref_ptr<World> world,
	osg::ref_ptr<World> worldLoading,
	osg::ref_ptr<GameSettings> settings)
		: NodeCallback(),
		  _viewer(viewer),
		  _world(world),
		  _worldLoading(worldLoading),
		  _gameSettings(settings),
		  _lastSimulationTime(0.0),
		  _gameEnded(false),
		  _isLoading(false)
{
	initialState->initialize(_world, _gameSettings);

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

		if (!_isLoading && (*it)->isLoadingState())
		{
			_isLoading = true;
			
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
				popState();
				break;
			case PUSH:
				pushState(se->referencedState);
				break;
			case REPLACE:
				popState();
				pushState(se->referencedState);
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

bool UpdateStateCallback::gameEnded()
{
	return _gameEnded;
}

void UpdateStateCallback::popState()
{
	_stateStack.pop_back();
}

void UpdateStateCallback::pushState(osg::ref_ptr<GameState> state)
{
	_stateStack.push_back(state);
}