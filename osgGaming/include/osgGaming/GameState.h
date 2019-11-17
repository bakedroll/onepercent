#pragma once

#include <osgGaming/AbstractGameState.h>

namespace osgGaming
{
	class GameState : public AbstractGameState
	{
		public:
			GameState();

			bool isLoadingState() const override final;
	};
}