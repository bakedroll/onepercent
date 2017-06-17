#include "VirtualOverlay.h"

#include <assert.h>

namespace onep
{
  struct VirtualOverlay::Impl
  {
    Impl()
      : texture(new osg::Texture2D())
    {
      texture->setDataVariance(osg::Object::DYNAMIC);
      texture->setImage(new osg::Image());
      texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
      texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
      texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
      texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
      texture->setResizeNonPowerOfTwoHint(false);
    }

    osg::ref_ptr<osg::Texture2D> texture;
    QImage image;
  };

  VirtualOverlay::VirtualOverlay()
    : QWidget(nullptr)
    , m(new Impl())
  {
    setAttribute(Qt::WA_DontShowOnScreen);
    setAttribute(Qt::WA_TranslucentBackground);
  }

  VirtualOverlay::~VirtualOverlay()
  {
  }

  osg::ref_ptr<osg::Texture2D> VirtualOverlay::getTexture()
  {
    return m->texture;
  }

  void VirtualOverlay::renderToTexture()
  {
    int width = m->texture->getTextureWidth();
    int height = m->texture->getTextureHeight();

    QPixmap pix(width, height);
    pix.fill(QColor(0,0,0,0));

    render(&pix);

    osg::Image* image = m->texture->getImage();
    assert(image);

    m->image = pix.toImage().convertToFormat(QImage::Format_RGBA8888);
    image->setImage(width, height, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, m->image.bits(), osg::Image::NO_DELETE);
  }
}
