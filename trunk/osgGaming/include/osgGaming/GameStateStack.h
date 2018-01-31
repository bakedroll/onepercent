#pragma once

#include <vector>

#include <osgGaming/AbstractGameState.h>

namespace osgGaming
{
	class GameStateStack
	{
	public:
		typedef std::vector<osg::ref_ptr<AbstractGameState>> AbstractGameStateList;

		GameStateStack();

		bool attachRequired();
		bool isEmpty();

		void begin(AbstractGameState::StateBehavior behavior, bool onlyInitialized = true);
		bool next();
		bool isTop();
		
		bool hasBehavior(osg::ref_ptr<AbstractGameState> state, AbstractGameState::StateBehavior behavior);

		osg::ref_ptr<AbstractGameState> get();

		void popState();
    void pushState(osg::ref_ptr<AbstractGameState> state);
		void pushStates(AbstractGameStateList states);
		void replaceState(AbstractGameStateList states);

	private:
		AbstractGameStateList _stateStack;

		bool _attachRequired;

		bool _itOnlyInitialized;
		AbstractGameState::StateBehavior _itBehavior;
		AbstractGameStateList::iterator _itCurrent;
		AbstractGameStateList::iterator _itNext;
		osg::ref_ptr<AbstractGameState> _itTop;

	};
}