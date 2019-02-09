#pragma once

#include "widgets/VirtualOverlay.h"

#include <osgGaming/Injector.h>

namespace onep
{
  class SkillsWidget : public VirtualOverlay
  {
    Q_OBJECT

  public:
    SkillsWidget(osgGaming::Injector& injector);
    ~SkillsWidget();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}