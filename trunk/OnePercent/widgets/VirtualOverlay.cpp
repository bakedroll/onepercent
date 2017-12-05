#include "VirtualOverlay.h"

#include <osg/Geode>
#include <osg/Switch>

#include <osgGaming/Helper.h>

#include <assert.h>

namespace onep
{
  struct VirtualOverlay::Impl
  {
    Impl()
      : rootNode(new osg::Switch())
      , transform(new osg::MatrixTransform())
      , geode(new osg::Geode())
      , texture(new osg::Texture2D())
      , bShouldBeVisible(true)
    {
      texture->setDataVariance(osg::Object::DYNAMIC);
      texture->setImage(new osg::Image());
      texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
      texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
      texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
      texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
      texture->setResizeNonPowerOfTwoHint(false);

      geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

      transform->addChild(geode);
      rootNode->addChild(transform, false);
    }

    void placeAt(int x, int y)
    {
      osg::Matrix mat = osg::Matrix::identity();
      mat.setTrans(x, y, 0);

      transform->setMatrix(mat);
    }

    void rescale(int width, int height)
    {
      // resize virtual overlay
      if (geode->getNumDrawables() > 0)
        geode->removeDrawable(geode->getDrawable(0));

      osg::ref_ptr<osg::Geometry> geometry = osgGaming::createQuadGeometry(0.0f, float(width) - 1.0f, float(height) - 1.0f, 0.0f, 0.0f, osgGaming::QuadOrientation::XY, false);
      geode->addDrawable(geometry);

      pixmap = QPixmap(width, height);
      texture->setTextureSize(width, height);
    }

    osg::ref_ptr<osg::Switch> rootNode;
    osg::ref_ptr<osg::MatrixTransform> transform;
    osg::ref_ptr<osg::Geode> geode;
    osg::ref_ptr<osg::Texture2D> texture;
    QPixmap pixmap;
    QImage image;

    bool bShouldBeVisible;
  };

  VirtualOverlay::VirtualOverlay()
    : QWidget(nullptr)
    , m(new Impl())
  {
    setAttribute(Qt::WA_DontShowOnScreen);
    setAttribute(Qt::WA_TranslucentBackground);

    QRect geo = geometry();

    m->placeAt(geo.x(), geo.y());
    m->rescale(geo.width(), geo.height());
  }

  VirtualOverlay::~VirtualOverlay()
  {
  }

  osg::ref_ptr<osg::Node> VirtualOverlay::getOsgNode()
  {
    return m->rootNode;
  }

  void VirtualOverlay::renderToTexture()
  {
    int width = m->texture->getTextureWidth();
    int height = m->texture->getTextureHeight();

    m->pixmap.fill(QColor(0,0,0,0));
    render(&m->pixmap);

    osg::Image* image = m->texture->getImage();
    assert(image);

    m->image = m->pixmap.toImage().convertToFormat(QImage::Format_RGBA8888);
    image->setImage(width, height, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, m->image.bits(), osg::Image::NO_DELETE);
  }

  void VirtualOverlay::setGeometry(const QRect& geo)
  {
    QRect oldGeo = geometry();
    
    // position changes
    if (geo.x() != oldGeo.x() || geo.y() != oldGeo.y())
      m->placeAt(geo.x(), geo.y());

    // size changes
    if (geo.width() != oldGeo.width() || geo.height() != oldGeo.height())
      m->rescale(geo.width(), geo.height());

    QWidget::setGeometry(geo);
  }

  void VirtualOverlay::setGeometry(int x, int y, int w, int h)
  {
    setGeometry(QRect(x, y, w, h));
  }

  void VirtualOverlay::setVisible(bool visible)
  {
    m->bShouldBeVisible = visible;
    m->rootNode->setChildValue(m->transform, visible);

    QWidget::setVisible(visible);
  }

  bool VirtualOverlay::shouldBeVisible()
  {
    return m->bShouldBeVisible;
  }
}
