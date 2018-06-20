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
    ~MainFrameWidget();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  signals:
    void clickedButtonSkills();
    void clickedButtonDebug();

  };
}