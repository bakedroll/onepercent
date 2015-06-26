#pragma once

#include "GlobeCameraState.h"

namespace onep
{
	class MainMenuState : public GlobeCameraState
	{
	public:
		MainMenuState();

		virtual void initialize() override;

		virtual osgGaming::GameState::StateEvent* update() override;

		virtual void onUIMClickedEvent(osgGaming::UserInteractionModel* model) override;

	protected:
		virtual osg::ref_ptr<osgGaming::Hud> newHud() override;
	};
}