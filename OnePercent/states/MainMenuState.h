#pragma once

#include "GlobeCameraState.h"

#include <osgHelper/ioc/Injector.h>

namespace onep
{
	class MainMenuState : public GlobeCameraState
	{
	public:
		MainMenuState(osgHelper::ioc::Injector& injector);
    virtual ~MainMenuState();

    void onInitialize(QPointer<QtOsgBridge::MainWindow> mainWindow, const SimulationData& data) override;
    void onUpdate(const SimulationData& data) override;
    void onExit() override;

  protected:
    void onResizeEvent(QResizeEvent* event) override;
    bool onKeyEvent(QKeyEvent* event) override;

    void setupUi(QPointer<QtOsgBridge::MainWindow> mainWindow);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
	};
}
