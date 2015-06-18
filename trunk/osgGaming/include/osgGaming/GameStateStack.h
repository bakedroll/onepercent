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

		AbstractGameStateList* getRunningStates();
		AbstractGameStateList* getNewRunningStates();

		void popState();
		void pushStates(AbstractGameStateList states);
		void replaceState(AbstractGameStateList states);

	private:
		AbstractGameStateList _stateStack;
		AbstractGameStateList _runningStateStack;
		AbstractGameStateList _newRunningStateStack;

		bool _attachRequired;
	};
}