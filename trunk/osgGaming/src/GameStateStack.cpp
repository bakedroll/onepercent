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
		_attachRequired = false;

	return res;
}

bool GameStateStack::isEmpty()
{
	return _stateStack.empty();
}

void GameStateStack::begin(AbstractGameState::StateBehavior behavior, bool onlyInitialized)
{
	_itBehavior = behavior;
	_itOnlyInitialized = onlyInitialized;
	_itNext = _stateStack.begin();
	_itTop = *(--_stateStack.end());
}

bool GameStateStack::next()
{
	while (_itNext != _stateStack.end())
	{
		if (hasBehavior(*_itNext, _itBehavior) && (!_itOnlyInitialized || _itNext->get()->isInitialized()))
		{
			break;
		}

		_itNext->get()->setDirty(_itBehavior);
		++_itNext;
	}

	if (_itNext != _stateStack.end())
	{
		_itCurrent = _itNext;
		_itNext = _itCurrent + 1;
		return true;
	}

	return false;
}

bool GameStateStack::isTop()
{
	return _itCurrent->get() == _itTop;
}

bool GameStateStack::hasBehavior(osg::ref_ptr<AbstractGameState> state, AbstractGameState::StateBehavior behavior)
{
	char props = state->getProperties();

	bool isTop = state == _itTop;

	if (behavior == AbstractGameState::UPDATE)
	{
		if ((props & AbstractGameState::PROP_UPDATE_ALWAYS) || (isTop && (props & AbstractGameState::PROP_UPDATE_TOP)))
		{
			return true;
		}
	}
	else if (behavior == AbstractGameState::GUIEVENT)
	{
		if ((props & AbstractGameState::PROP_GUIEVENTS_ALWAYS) || (isTop && (props & AbstractGameState::PROP_GUIEVENTS_TOP)))
		{
			return true;
		}
	}
	else if (behavior == AbstractGameState::ALL)
	{
		if (((props & AbstractGameState::PROP_UPDATE_ALWAYS) || (isTop && (props & AbstractGameState::PROP_UPDATE_TOP))) ||
			((props & AbstractGameState::PROP_GUIEVENTS_ALWAYS) || (isTop && (props & AbstractGameState::PROP_GUIEVENTS_TOP))))
		{
			return true;
		}
	}

	return false;
}


ref_ptr<AbstractGameState> GameStateStack::get()
{
	return *_itCurrent;
}

void GameStateStack::popState()
{
	_attachRequired = true;
	_stateStack.pop_back();
}

void GameStateStack::pushState(osg::ref_ptr<AbstractGameState>& state)
{
  _attachRequired = true;
  _stateStack.push_back(state);
}

void GameStateStack::pushStates(AbstractGameState::AbstractGameStateRefList states)
{
	_attachRequired = true;

  for (AbstractGameState::AbstractGameStateRefList::iterator it = states.begin(); it != states.end(); ++it)
	{
		_stateStack.push_back(*it);
	}
}

void GameStateStack::replaceState(AbstractGameState::AbstractGameStateRefList states)
{
	popState();
	pushStates(states);
}

void GameStateStack::clear()
{
  _stateStack.clear();
}