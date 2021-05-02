#pragma once

#include "GlobeCameraState.h"

#include <memory>

namespace onep
{
	class GlobeInteractionState : public GlobeCameraState
	{
	public:
		GlobeInteractionState(osgHelper::ioc::Injector& injector);
		~GlobeInteractionState();

		void onInitialize(QPointer<QtOsgBridge::MainWindow> mainWindow, const SimulationData& data) override;
		void onUpdate(const SimulationData& data) override;
		void onExit() override;

	protected:
		bool onKeyEvent(QKeyEvent* event) override;
		bool onMouseEvent(QMouseEvent* event) override;
		bool onWheelEvent(QWheelEvent* event) override;

    void onDragBegin(Qt::MouseButton button, const osg::Vec2f& origin) override;
    void onDragMove(Qt::MouseButton button, const osg::Vec2f& origin,
                    const osg::Vec2f& position, const osg::Vec2f& change) override;
    void onDragEnd(Qt::MouseButton button, const osg::Vec2f& origin, const osg::Vec2f& position) override;
    void onResizeEvent(QResizeEvent* event) override;

    void setupUi(const QPointer<QtOsgBridge::MainWindow>& mainWindow);

	private:
		struct Impl;
		std::unique_ptr<Impl> m;

	};
}
