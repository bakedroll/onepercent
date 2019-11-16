#pragma once

#include "VirtualOverlay.h"

#include <osgGaming/Injector.h>

namespace onep
{
  class MainFrameWidget : public VirtualOverlay
  {
    Q_OBJECT

  public:
    MainFrameWidget(osgGaming::Injector& injector);
    virtual ~MainFrameWidget();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  Q_SIGNALS:
    void clickedButtonDebug();
    void toggledWidgetEnabled(bool enabled);

  };
}