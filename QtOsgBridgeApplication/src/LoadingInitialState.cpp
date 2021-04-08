#include "LoadingInitialState.h"
#include "InitialState.h"

#include <QtOsgBridge/OverlayCompositor.h>

#include <QPointer>
#include <QVBoxLayout>
#include <QLabel>

LoadingInitialState::LoadingInitialState(osgHelper::ioc::Injector& injector)
  : LoadingState(injector)
{
}

void LoadingInitialState::onInitializeLoading(QPointer<QtOsgBridge::MainWindow> mainWindow)
{
  m_compositor = mainWindow->getViewWidget()->getOverlayCompositor();

  QVBoxLayout* layout = new QVBoxLayout();
  layout->addWidget(new QLabel("Loading..."));
  layout->addStretch(1);

  m_overlay = new QtOsgBridge::VirtualOverlay();
  m_overlay->setLayout(layout);
  m_overlay->setGeometry(0, 0, 300, 300);
  m_overlay->setStyleSheet("color: #fff; font-size: 36pt;");

  m_compositor->addVirtualOverlay(m_overlay);

  m_overlay->show();
}

void LoadingInitialState::onExitLoading()
{
  m_compositor->removeVirtualOverlay(m_overlay);
}

void LoadingInitialState::onLoading()
{
  QThread::msleep(1000);
}

void LoadingInitialState::onRequestNewStates()
{
  requestNewEventState<InitialState>(NewEventStateMode::ExitCurrent);
}
