#pragma once

#include <osgGaming/Injector.h>

#include <QDialog>
#include <memory>

namespace onep
{
  class DebugWindow : public QDialog
  {
    Q_OBJECT

  public:
    DebugWindow(
      osgGaming::Injector& injector, 
      QWidget* parent = nullptr);
    ~DebugWindow();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}