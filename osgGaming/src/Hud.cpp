#include <osgGaming/Injector.h>
#include <osgGaming/Hud.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/Helper.h>

namespace osgGaming
{

  struct Hud::Impl
  {
    Impl(Injector& injector)
      : resourceManager(injector.inject<ResourceManager>())
    {}

    osg::ref_ptr<ResourceManager> resourceManager;
    osg::ref_ptr<osg::Projection> projection;
    osg::ref_ptr<osg::MatrixTransform> modelViewTransform;

    osg::Vec2f resolution;
  };

  Hud::Hud(Injector& injector)
    : Referenced()
    , m(new Impl(injector))
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

  Hud::~Hud()
  {
  }

  osg::ref_ptr<osg::Projection> Hud::getProjection()
  {
    return m->projection;
  }

  osg::ref_ptr<osg::MatrixTransform> Hud::getModelViewTransform()
  {
    return m->modelViewTransform;
  }

  void Hud::updateResolution(osg::Vec2f resolution)
  {
    m->resolution = resolution;
    m->projection->setMatrix(osg::Matrix::ortho2D(0.0, double(m->resolution.x()) - 1.0, double(m->resolution.y()) - 1.0, 0.0));
  }
}
