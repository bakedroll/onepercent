#include "MainWindow.h"
#include "OsgWidget.h"

#include <QVBoxLayout>
#include <QPushButton>

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
    QPushButton* button = new QPushButton();
    button->setWindowFlags(Qt::FramelessWindowHint);
    button->setAttribute(Qt::WA_NoSystemBackground);
    button->setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(button);

    m->osgWidget = new OsgWidget(viewer);
    m->osgWidget->setLayout(mainLayout);

    setMinimumSize(1024, 768);
    setCentralWidget(m->osgWidget);
  }

  MainWindow::~MainWindow()
  {
  }
}