#pragma once

#include <osgHelper/ioc/Injector.h>

#include <QtOsgBridge/VirtualOverlay.h>

namespace onep
{
  class MainFrameWidget : public QtOsgBridge::VirtualOverlay
  {
    Q_OBJECT

  public:
    MainFrameWidget(osgHelper::ioc::Injector& injector);
    virtual ~MainFrameWidget();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  Q_SIGNALS:
    void clickedButtonDebug();
    void toggledWidgetEnabled(bool enabled);

  };
}