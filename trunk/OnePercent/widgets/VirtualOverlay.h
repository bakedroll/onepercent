#pragma once

#include <QWidget>

#include <osg/MatrixTransform>
#include <osg/Texture2D>

#include <memory>

namespace onep
{
  class VirtualOverlay : public QWidget
  {
    Q_OBJECT

  public:
    VirtualOverlay();
    ~VirtualOverlay();

    osg::ref_ptr<osg::Node> getOsgNode();

    void renderToTexture();

    void setGeometry(const QRect& geometry);
    void setGeometry(int x, int y, int w, int h);

    virtual void setVisible(bool visible) override;
    bool shouldBeVisible();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };
}
