#pragma once

#include <QMainWindow>
#include <memory>

#include "OsgWidget.h"

namespace onep
{
  class MainWindow : public QMainWindow
  {
  public:
    MainWindow(osg::ref_ptr<osgViewer::CompositeViewer> viewer);
    ~MainWindow();

    OsgWidget* getViewWidget();

    void shutdown();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}