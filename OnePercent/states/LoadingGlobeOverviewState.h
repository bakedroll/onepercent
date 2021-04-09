#pragma once

#include <osgHelper/ioc/Injector.h>

#include <QtOsgBridge/LoadingState.h>

#include <memory>

namespace onep
{
	class LoadingGlobeOverviewState : public QtOsgBridge::LoadingState
	{
	public:
    LoadingGlobeOverviewState(osgHelper::ioc::Injector& injector);
		~LoadingGlobeOverviewState();

		void onUpdate(const SimulationData& data) override;

	protected:
		void onInitializeLoading(QPointer<QtOsgBridge::MainWindow> mainWindow, const SimulationData& data) override;
		void onLoading() override;
		void onExitLoading() override;
		void onRequestNewStates() override;

		void onResizeEvent(QResizeEvent* event) override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m;

	};
}
