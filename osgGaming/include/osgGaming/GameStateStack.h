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
		bool isEmpty();

		void begin(AbstractGameState::StateBehavior behavior, bool onlyInitialized = true);
		bool next();
		bool isTop();
		
		bool hasBehavior(osg::ref_ptr<AbstractGameState> state, AbstractGameState::StateBehavior behavior);

		osg::ref_ptr<AbstractGameState> get();

		void popState();
    void pushState(osg::ref_ptr<AbstractGameState>& state);
		void pushStates(AbstractGameState::AbstractGameStateRefList states);
    void replaceState(AbstractGameState::AbstractGameStateRefList states);

    void clear();

	private:
    AbstractGameState::AbstractGameStateList _stateStack;

		bool _attachRequired;

		bool _itOnlyInitialized;
		AbstractGameState::StateBehavior _itBehavior;
    AbstractGameState::AbstractGameStateList::iterator _itCurrent;
    AbstractGameState::AbstractGameStateList::iterator _itNext;
		AbstractGameState* _itTop;

	};
}