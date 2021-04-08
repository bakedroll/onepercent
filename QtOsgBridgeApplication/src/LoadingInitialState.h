#pragma once

#include <QtOsgBridge/LoadingState.h>
#include <QtOsgBridge/VirtualOverlay.h>

#include <QPointer>

class LoadingInitialState : public QtOsgBridge::LoadingState
{
public:
  explicit LoadingInitialState(osgHelper::ioc::Injector& injector);

protected:
  void onInitializeLoading(QPointer<QtOsgBridge::MainWindow> mainWindow) override;
  void onExitLoading() override;
  void onLoading()  override;
  void onRequestNewStates() override;

private:
  QPointer<QtOsgBridge::VirtualOverlay>        m_overlay;
  osg::ref_ptr<QtOsgBridge::OverlayCompositor> m_compositor;

};