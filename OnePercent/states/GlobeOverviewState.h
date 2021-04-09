#pragma once

#include <osgHelper/ioc/Injector.h>

#include <QtOsgBridge/EventProcessingState.h>

#include "nodes/GlobeOverviewWorld.h"

namespace onep
{
	class GlobeOverviewState : public QtOsgBridge::EventProcessingState
	{
	public:
		GlobeOverviewState(osgHelper::ioc::Injector& injector);
    ~GlobeOverviewState() override;

		void onInitialize(QPointer<QtOsgBridge::MainWindow> mainWindow, const SimulationData& data) override;
		void onUpdate(const SimulationData& data) override;

	protected:
		bool onKeyEvent(QKeyEvent* event) override;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
