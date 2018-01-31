#pragma once

#include "GlobeCameraState.h"

#include <osgGaming/Injector.h>

#include <osgGaming/UIElement.h>

namespace onep
{
	class MainMenuState : public GlobeCameraState
	{
	public:
		MainMenuState(osgGaming::Injector& injector);
    ~MainMenuState();

		virtual void initialize() override;

		virtual osgGaming::GameState::StateEvent* update() override;
    virtual void onResizeEvent(float width, float height) override;
    virtual void onKeyPressedEvent(int key) override;

	protected:
		virtual osg::ref_ptr<osgGaming::Hud> injectHud(osgGaming::Injector& injector, osg::ref_ptr<osgGaming::View> view) override;

    void setupUi();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
	};
}
