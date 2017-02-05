#pragma once

#include <QMainWindow>
#include <memory>
#include <osgGaming/View.h>

#include "OsgWidget.h"

namespace onep
{
  class MainWindow : public QMainWindow
  {
  public:
    MainWindow(osg::ref_ptr<osgViewer::CompositeViewer> viewer);
    ~MainWindow();

    OsgWidget* getViewWidget();

  protected:
    virtual void closeEvent(QCloseEvent *event) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}