#include <osgGaming/Injector.h>
#include <osgGaming/Hud.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/Helper.h>

namespace osgGaming
{

  struct Hud::Impl
  {
    Impl() = default;

    osg::ref_ptr<osg::Projection>      projection;
    osg::ref_ptr<osg::MatrixTransform> modelViewTransform;

    osg::Vec2f resolution;
  };

  Hud::Hud(Injector& injector)
    : Referenced()
    , m(new Impl())
  {
    osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();
    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    stateSet->setRenderBinDetails(10, "RenderBin");

    m->modelViewTransform = new osg::MatrixTransform();
    m->modelViewTransform->setMatrix(osg::Matrix::identity());
    m->modelViewTransform->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    m->projection = new osg::Projection();
    m->projection->addChild(m->modelViewTransform);
    m->projection->setStateSet(stateSet);
  }

  Hud::~Hud() = default;

  osg::ref_ptr<osg::Projection> Hud::getProjection() const
  {
    return m->projection;
  }

  osg::ref_ptr<osg::MatrixTransform> Hud::getModelViewTransform() const
  {
    return m->modelViewTransform;
  }

  void Hud::updateResolution(const osg::Vec2f& resolution)
  {
    m->resolution = resolution;
    m->projection->setMatrix(osg::Matrix::ortho2D(0.0, static_cast<double>(m->resolution.x()) - 1.0,
                                                  static_cast<double>(m->resolution.y()) - 1.0, 0.0));
  }

  }  // namespace osgGaming
