#pragma once

#include <QtOpenGL/QGLWidget>
#include <memory>

#include <osgViewer/CompositeViewer>
#include <osgGaming/InputManager.h>

namespace onep
{
  class OverlayCompositor;
  class VirtualOverlay;

  class OsgWidget : public QGLWidget, public osgGaming::InputManager
  {
    Q_OBJECT

  public:
    OsgWidget(osg::ref_ptr<osgViewer::CompositeViewer> viewer, QWidget* parent = nullptr, Qt::WindowFlags f = 0);
    ~OsgWidget();

    void setOverlayCompositor(std::shared_ptr<OverlayCompositor> compositor);

  protected:
    virtual void paintGL() override;
    virtual void resizeGL(int width, int height) override;

    virtual void	keyPressEvent(QKeyEvent* event) override;
    virtual void	keyReleaseEvent(QKeyEvent* event) override;
    virtual void	mouseMoveEvent(QMouseEvent* event) override;
    virtual void	mousePressEvent(QMouseEvent* event) override;
    virtual void	mouseReleaseEvent(QMouseEvent* event) override;
    virtual void	wheelEvent(QWheelEvent* event) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };

}
