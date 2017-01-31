#include "OsgWidget.h"

#include <assert.h>

#include <osgGaming/View.h>
#include <QTimer>

namespace onep
{
  struct OsgWidget::Impl
  {
    Impl(osg::ref_ptr<osgViewer::CompositeViewer> viewer)
      : viewer(viewer)
    {
    }

    osg::ref_ptr<osgViewer::CompositeViewer> viewer;
    osg::ref_ptr<osgGaming::View> view;
    osg::ref_ptr<osg::Camera> camera;
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow;

    QTimer updateTimer;
  };

  OsgWidget::OsgWidget(osg::ref_ptr<osgViewer::CompositeViewer> viewer, QWidget* parent, Qt::WindowFlags f)
    : QGLWidget(parent, nullptr, f)
    , m(new Impl(viewer))
  {
    //setWindowFlags(Qt::FramelessWindowHint);
    //setAttribute(Qt::WA_NoSystemBackground);
    //setAttribute(Qt::WA_TranslucentBackground);

    m->graphicsWindow = new osgViewer::GraphicsWindowEmbedded(x(), y(), width(), height());
    m->view = dynamic_cast<osgGaming::View*>(viewer->getView(0));
    m->camera = m->view->getSceneCamera();

    //float aspectRatio = static_cast<float>(this->width() / 2) / static_cast<float>(this->height());
    //m->camera->setClearColor(osg::Vec4(0.f, 0.f, 1.f, 1.f));
    //m->camera->setProjectionMatrixAsPerspective(30.f, aspectRatio, 1.f, 1000.f);


    if (!m->camera || !m->view)
    {
      assert(false && "Please set camera");
      return;
    }

    osg::ref_ptr<osg::Camera> hudcam = m->view->getHudCamera();

    m->camera->setGraphicsContext(m->graphicsWindow);
    hudcam->setGraphicsContext(m->graphicsWindow);

    m->camera->setViewport(0, 0, width(), height());
    hudcam->setViewport(0, 0, width(), height());

    m->updateTimer.setInterval(16);
    m->updateTimer.setSingleShot(false);
    
    connect(&m->updateTimer, SIGNAL(timeout()), this, SLOT(update()));

    m->updateTimer.start();
  }

  OsgWidget::~OsgWidget()
  {
  }

  void OsgWidget::paintEvent(QPaintEvent* paintEvent)
  {
    //QOpenGLWidget::paintEvent(paintEvent);

    makeCurrent();

    QPainter painter(this);
    // painter.setRenderHint(QPainter::Antialiasing);

    paintGL();

    painter.end();

    doneCurrent();
  }

  void OsgWidget::paintGL()
  {
    m->viewer->frame();
  }

  void OsgWidget::resizeGL(int width, int height)
  {
    m->graphicsWindow->getEventQueue()->windowResize(x(), y(), width, height);
    m->graphicsWindow->resized(x(), y(), width, height);

    m->camera->setViewport(0, 0, width, height);
    m->view->updateResolution(osg::Vec2f(width, height));
  }

  bool OsgWidget::event(QEvent* event)
  {
    bool handled = QGLWidget::event(event);

    return handled;
  }
}