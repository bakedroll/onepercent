#pragma once

#include <QWidget>

#include <osgHelper/ioc/Injector.h>

namespace onep
{
  class SkillsWidget : public QWidget
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
