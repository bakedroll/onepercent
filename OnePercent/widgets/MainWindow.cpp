#include "MainWindow.h"
#include "OsgWidget.h"

namespace onep
{
  struct MainWindow::Impl
  {
    OsgWidget* osgWidget;
  };

  MainWindow::MainWindow(osg::ref_ptr<osgViewer::CompositeViewer> viewer)
    : QMainWindow()
    , m(new Impl())
  {
    m->osgWidget = new OsgWidget(viewer);

    setMinimumSize(1024, 768);
    setCentralWidget(m->osgWidget);
  }

  MainWindow::~MainWindow()
  {
  }
}