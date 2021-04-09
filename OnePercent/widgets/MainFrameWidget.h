#pragma once

#include <QWidget>

#include <osgHelper/ioc/Injector.h>

namespace onep
{
  class MainFrameWidget : public QWidget
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