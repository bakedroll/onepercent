#pragma once

#include <osgGaming/AbstractGameState.h>

namespace osgGaming
{
	class GameState : public AbstractGameState
	{
		public:
			GameState();

			virtual bool isLoadingState() override final;
	};
}