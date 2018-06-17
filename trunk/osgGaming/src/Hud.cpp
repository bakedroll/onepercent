#include <osgGaming/Injector.h>
#include <osgGaming/Hud.h>
#include <osgGaming/FpsTextCallback.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/Helper.h>

#include <osg/Geode>

#include <osgText/Text>

namespace osgGaming
{

  struct Hud::Impl
  {
    Impl(Injector& injector)
      : resourceManager(injector.inject<ResourceManager>())
      , fpsEnabled(false)
    {}

    osg::ref_ptr<ResourceManager> resourceManager;

    osg::ref_ptr<osg::Projection> projection;
    osg::ref_ptr<osg::MatrixTransform> textTransform;
    osg::ref_ptr<osg::MatrixTransform> modelViewTransform;

    bool fpsEnabled;

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

  void Hud::setFpsEnabled(bool enabled)
  {
    if (enabled == m->fpsEnabled)
      return;

    m->fpsEnabled = enabled;

    if (!m->textTransform.valid())
    {
      osg::Matrix mat = osg::Matrix::identity();
      mat.makeScale(1.0f, -1.0f, 1.0f);
      mat.setTrans(10.0f, 10.0f, 0.0f);

      osg::ref_ptr<osgText::Text> fpsText = createTextNode("", 25.0f, m->resourceManager->loadDefaultFont());
      fpsText->setAlignment(osgText::TextBase::LEFT_TOP);
      fpsText->setAxisAlignment(osgText::Text::AxisAlignment::XY_PLANE);
      fpsText->setPosition(osg::Vec3f(0.0f, 0.0f, 0.0f));
      fpsText->setDataVariance(osg::Object::DYNAMIC);
      fpsText->setUpdateCallback(new FpsTextCallback());

      osg::ref_ptr<osg::Geode> textGeode = new osg::Geode();
      textGeode->addDrawable(fpsText);

      m->textTransform = new osg::MatrixTransform();
      m->textTransform->setMatrix(mat);
      m->textTransform->addChild(textGeode);
    }

    if (m->fpsEnabled == true)
      m->modelViewTransform->addChild(m->textTransform);
    else
      m->modelViewTransform->removeChild(m->textTransform);
  }

}
