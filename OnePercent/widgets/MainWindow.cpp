#include "MainWindow.h"
#include "OsgWidget.h"

#include <QFrame>
#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>

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
  QVBoxLayout* frameLayout = new QVBoxLayout();
  frameLayout->setContentsMargins(0, 0, 0, 0);

  QFrame* frame = new QFrame();
  frame->setLayout(frameLayout);

  m->osgWidget = new OsgWidget(viewer);
  frameLayout->addWidget(m->osgWidget);

  QRect desktopRect = QApplication::desktop()->screenGeometry();
  int winWidth = 1280;
  int winHeight = 768;

  setGeometry(QRect(desktopRect.width() / 2 - winWidth / 2, desktopRect.height() / 2 - winHeight / 2, winWidth, winHeight));

  setMinimumSize(winWidth, winHeight);
  setCentralWidget(frame);
}

MainWindow::~MainWindow()
{
  m->osgWidget->setParent(nullptr);
}

OsgWidget* MainWindow::getViewWidget()
{
  return m->osgWidget;
}

void MainWindow::shutdown()
{
  m->osgWidget->setParent(nullptr);
  close();
}

}
