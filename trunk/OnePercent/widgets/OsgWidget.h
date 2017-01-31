#pragma once

//#include <QtWidgets/QOpenGLWidget>
#include <QtOpenGL/QGLWidget>
#include <osgGaming/View.h>
#include <memory>

#include <osgViewer/CompositeViewer>
#include <QOpenGLWidget>

namespace onep
{
  class OsgWidget : public QGLWidget
  {
    Q_OBJECT

  public:
    OsgWidget(osg::ref_ptr<osgViewer::CompositeViewer> viewer, QWidget* parent = nullptr, Qt::WindowFlags f = 0);
    ~OsgWidget();

  protected:
    virtual void paintEvent(QPaintEvent* paintEvent) override;
    virtual void paintGL() override;
    virtual void resizeGL(int width, int height) override;

    virtual bool event(QEvent* event) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };

}