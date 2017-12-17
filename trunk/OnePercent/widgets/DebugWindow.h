#pragma once

#include "nodes/GlobeOverviewWorld.h"

#include <QDialog>
#include <memory>

namespace onep
{
  class DebugWindow : public QDialog
  {
    Q_OBJECT

  public:
    DebugWindow(osg::ref_ptr<GlobeOverviewWorld> world, QWidget* parent = nullptr);
    ~DebugWindow();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}