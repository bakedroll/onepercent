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

bool GameStateStack::isEmpty()
{
	return _stateStack.empty();
}

void GameStateStack::begin(AbstractGameState::StateBehavior behavior)
{
	_itBehavior = behavior;
	_itNext = _stateStack.begin();
	_itTop = *(--_stateStack.end());
}

bool GameStateStack::next()
{
	while (_itNext != _stateStack.end())
	{
		char props = _itNext->get()->getProperties();

		if (_itBehavior == AbstractGameState::UPDATE)
		{
			if ((props & AbstractGameState::PROP_UPDATE_ALWAYS) || ((_itNext->get() == _itTop.get()) && (props & AbstractGameState::PROP_UPDATE_TOP)))
			{
				break;
			}
		}
		else if (_itBehavior == AbstractGameState::GUIEVENT)
		{
			if ((props & AbstractGameState::PROP_GUIEVENTS_ALWAYS) || ((_itNext->get() == _itTop.get()) && (props & AbstractGameState::PROP_GUIEVENTS_TOP)))
			{
				break;
			}
		}
		else if (_itBehavior == AbstractGameState::UIMEVENT)
		{
			if ((props & AbstractGameState::PROP_UIMEVENTS_ALWAYS) || ((_itNext->get() == _itTop.get()) && (props & AbstractGameState::PROP_UIMEVENTS_TOP)))
			{
				break;
			}
		}

		_itNext->get()->dirty(_itBehavior);
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
	return _itCurrent->get() == _itTop.get();
}

ref_ptr<AbstractGameState> GameStateStack::get()
{
	return *_itCurrent;
}

ref_ptr<AbstractGameState> GameStateStack::top()
{
	return *(--_stateStack.end());
}

void GameStateStack::popState()
{
	_attachRequired = true;
	_stateStack.pop_back();
}

void GameStateStack::pushStates(AbstractGameStateList states)
{
	_attachRequired = true;

	for (AbstractGameStateList::iterator it = states.begin(); it != states.end(); ++it)
	{
		_stateStack.push_back(*it);
	}
}

void GameStateStack::replaceState(AbstractGameStateList states)
{
	popState();
	pushStates(states);
}