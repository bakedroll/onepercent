#pragma once

#include <osgHelper/ioc/Injector.h>

#include <QtOsgBridge/VirtualOverlay.h>

namespace onep
{
  class SkillsWidget : public QtOsgBridge::VirtualOverlay
  {
    Q_OBJECT

  public:
    SkillsWidget(osgHelper::ioc::Injector& injector);
    ~SkillsWidget();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}
