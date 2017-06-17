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

		virtual VirtualOverlay* createVirtualOverlay() override;

		virtual osgGaming::GameState::StateEvent* update() override;

	protected:
		virtual osg::ref_ptr<osgGaming::Hud> overrideHud(osg::ref_ptr<osgGaming::View> view) override;
	};
}
