#include <osgGaming/GameStateStack.h>

using namespace osg;
using namespace osgGaming;

GameStateStack::GameStateStack()
	: _attachRequired(false)
{

}

bool GameStateStack::attachRequired()
{
	bool res = _attachRequired;

	if (res)
	{
		_attachRequired = false;
	}

	return res;
}

GameStateStack::AbstractGameStateList* GameStateStack::getRunningStates()
{
	return &_runningStateStack;
}

GameStateStack::AbstractGameStateList* GameStateStack::getNewRunningStates()
{
	return &_newRunningStateStack;
}

void GameStateStack::popState()
{
	_attachRequired = true;

	_newRunningStateStack.clear();

	_stateStack.pop_back();
	_runningStateStack.pop_back();

	if (_stateStack.empty())
	{
		return;
	}

	ref_ptr<AbstractGameState> topState = *(--_stateStack.end());

	if (!(topState->getProperties() & AbstractGameState::PROP_RUN_ALWAYS))
	{
		_runningStateStack.push_back(topState);
		_newRunningStateStack.push_back(topState);
	}
}

void GameStateStack::pushStates(AbstractGameStateList states)
{
	_attachRequired = true;

	_newRunningStateStack.clear();

	ref_ptr<AbstractGameState> topPushedState = *(--states.end());

	if (!_stateStack.empty())
	{
		ref_ptr<AbstractGameState> topState = *(--_stateStack.end());

		if (!(topState->getProperties() & AbstractGameState::PROP_RUN_ALWAYS))
		{
			_runningStateStack.pop_back();
		}
	}

	for (AbstractGameStateList::iterator it = states.begin(); it != states.end(); ++it)
	{
		unsigned char props = (*it)->getProperties();

		if (!(props & AbstractGameState::PROP_ENABLED))
		{
			continue;
		}

		if ((props & AbstractGameState::PROP_RUN_ALWAYS) || (topPushedState == (*it)))
		{
			_runningStateStack.push_back(*it);
			_newRunningStateStack.push_back(*it);
		}

		_stateStack.push_back(*it);
	}
}

void GameStateStack::replaceState(AbstractGameStateList states)
{
	popState();
	pushStates(states);
}