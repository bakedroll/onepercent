#include <osgGaming/GameStateStack.h>
#include "osgGaming/Helper.h"

using namespace osg;
using namespace osgGaming;

GameStateStack::GameStateStack()
  : m_attachRequired(false),
    m_itOnlyInitialized(false),
    m_itBehavior(),
    m_itTop(nullptr)
{
}

bool GameStateStack::attachRequired()
{
	bool res = m_attachRequired;

	if (res)
		m_attachRequired = false;

	return res;
}

bool GameStateStack::isEmpty() const
{
	return _stateStack.empty();
}

void GameStateStack::begin(AbstractGameState::StateBehavior behavior, bool onlyInitialized)
{
  m_itBehavior        = behavior;
  m_itOnlyInitialized = onlyInitialized;
  m_itNext            = _stateStack.begin();
  m_itTop             = *(--_stateStack.end());
}

bool GameStateStack::next()
{
	while (m_itNext != _stateStack.end())
	{
		if (hasBehavior(*m_itNext, m_itBehavior) && (!m_itOnlyInitialized || m_itNext->get()->isInitialized()))
		{
			break;
		}

		m_itNext->get()->setDirty(m_itBehavior);
		++m_itNext;
	}

	if (m_itNext != _stateStack.end())
	{
		_itCurrent = m_itNext;
		m_itNext = _itCurrent + 1;
		return true;
	}

	return false;
}

bool GameStateStack::isTop() const
{
	return _itCurrent->get() == m_itTop;
}

bool GameStateStack::hasBehavior(osg::ref_ptr<AbstractGameState> state, AbstractGameState::StateBehavior behavior)
{
	auto props = state->getProperties();
	auto isTop = (state == m_itTop);

	if (behavior == AbstractGameState::StateBehavior::Update)
	{
    if ((props & underlying(AbstractGameState::StateProperties::UpdateAlways)) ||
        (isTop && (props & underlying(AbstractGameState::StateProperties::UpdateTop))))
    {
      return true;
    }
  }
	else if (behavior == AbstractGameState::StateBehavior::GuiEvent)
  {
    if ((props & underlying(AbstractGameState::StateProperties::GuiEventsAlways)) ||
        (isTop && (props & underlying(AbstractGameState::StateProperties::GuiEventsTop))))
    {
      return true;
    }
  }
  else if (behavior == AbstractGameState::StateBehavior::All)
	{
    if (((props & underlying(AbstractGameState::StateProperties::UpdateAlways)) ||
         (isTop && (props & underlying(AbstractGameState::StateProperties::UpdateTop)))) ||
        ((props & underlying(AbstractGameState::StateProperties::GuiEventsAlways)) ||
         (isTop && (props & underlying(AbstractGameState::StateProperties::GuiEventsTop)))))
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
	m_attachRequired = true;
	_stateStack.pop_back();
}

void GameStateStack::pushState(osg::ref_ptr<AbstractGameState>& state)
{
  m_attachRequired = true;
  _stateStack.push_back(state);
}

void GameStateStack::pushStates(AbstractGameState::AbstractGameStateRefList states)
{
	m_attachRequired = true;

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