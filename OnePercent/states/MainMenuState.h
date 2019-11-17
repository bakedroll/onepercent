#pragma once

#include "GlobeCameraState.h"

#include <osgGaming/Injector.h>

namespace onep
{
	class MainMenuState : public GlobeCameraState
	{
	public:
		MainMenuState(osgGaming::Injector& injector);
    virtual ~MainMenuState();

		void initialize() override;

    StateEvent* update() override;
    void        onResizeEvent(float width, float height) override;
    void        onKeyPressedEvent(int key) override;

  protected:
    osg::ref_ptr<osgGaming::Hud> injectHud(osgGaming::Injector&                 injector,
                                           const osg::ref_ptr<osgGaming::View>& view) override;

    void setupUi();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
	};
}
