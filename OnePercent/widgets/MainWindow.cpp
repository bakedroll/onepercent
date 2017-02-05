#include "MainWindow.h"
#include "OsgWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>

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

  QPushButton* button = new QPushButton();
  button->setWindowFlags(Qt::FramelessWindowHint);
  button->setAttribute(Qt::WA_NoSystemBackground);
  button->setAttribute(Qt::WA_TranslucentBackground);

  QVBoxLayout* mainLayout = new QVBoxLayout();

  m->osgWidget = new OsgWidget(viewer);
  m->osgWidget->setLayout(mainLayout);

  frameLayout->addWidget(m->osgWidget);

  setMinimumSize(1280, 768);
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