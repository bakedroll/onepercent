#pragma once

#include "GlobeCameraState.h"

#include <osgGaming/UIElement.h>

namespace onep
{
	class MainMenuState : public GlobeCameraState
	{
	public:
		MainMenuState();

		virtual void initialize() override;

		virtual osgGaming::GameState::StateEvent* update() override;

		virtual void onUIClickedEvent(osg::ref_ptr<osgGaming::UIElement> uiElement) override;

	protected:
		virtual osg::ref_ptr<osgGaming::Hud> overrideHud() override;
	};
}