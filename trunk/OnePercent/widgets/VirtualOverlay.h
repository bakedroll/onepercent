#pragma once

#include <QWidget>

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

    osg::ref_ptr<osg::Texture2D> getTexture();
    void renderToTexture();

  private slots:
    void test();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };
}
