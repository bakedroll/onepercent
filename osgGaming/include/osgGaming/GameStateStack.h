#pragma once

#include <vector>

#include <osgGaming/AbstractGameState.h>

namespace osgGaming
{
	class GameStateStack
	{
	public:
		GameStateStack();

		bool attachRequired();
		bool isEmpty() const;

		void begin(AbstractGameState::StateBehavior behavior, bool onlyInitialized = true);
		bool next();
		bool isTop() const;
		
		bool hasBehavior(osg::ref_ptr<AbstractGameState> state, AbstractGameState::StateBehavior behavior);

		osg::ref_ptr<AbstractGameState> get();

		void popState();
    void pushState(osg::ref_ptr<AbstractGameState>& state);
		void pushStates(AbstractGameState::AbstractGameStateRefList states);
    void replaceState(AbstractGameState::AbstractGameStateRefList states);

    void clear();

	private:
    AbstractGameState::AbstractGameStateList _stateStack;

		bool m_attachRequired;

		bool m_itOnlyInitialized;
		AbstractGameState::StateBehavior m_itBehavior;
    AbstractGameState::AbstractGameStateList::iterator _itCurrent;
    AbstractGameState::AbstractGameStateList::iterator m_itNext;
		AbstractGameState* m_itTop;

	};
}