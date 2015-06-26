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

		void begin(AbstractGameState::StateBehavior behavior);
		bool next();
		bool isTop();
		osg::ref_ptr<AbstractGameState> get();

		osg::ref_ptr<AbstractGameState> top();

		void popState();
		void pushStates(AbstractGameStateList states);
		void replaceState(AbstractGameStateList states);

	private:
		AbstractGameStateList _stateStack;

		bool _attachRequired;

		AbstractGameState::StateBehavior _itBehavior;
		AbstractGameStateList::iterator _itCurrent;
		AbstractGameStateList::iterator _itNext;
		osg::ref_ptr<AbstractGameState> _itTop;

	};
}