#include "BackgroundModel.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Point>
#include <osg/PointSprite>
#include <osg/BlendFunc>
#include <osg/BlendEquation>
#include <osg/Billboard>

#include <osgGaming/ResourceManager.h>
#include <osgGaming/TextureFactory.h>
#include <osgGaming/Helper.h>
#include <osgGaming/ByteStream.h>

namespace onep
{
  struct BackgroundModel::Impl
  {
    Impl(osgGaming::Injector& injector, BackgroundModel* b)
      : base(b)
      , resourceManager(injector.inject<osgGaming::ResourceManager>())
      , textureFactory(injector.inject<osgGaming::TextureFactory>())
    {}

    void makeStars(std::string filename)
    {
      char* bytes = resourceManager->loadBinary(filename);

      osg::ref_ptr<osg::Geode> geode = new osg::Geode();
      osg::ref_ptr<osg::Geometry> geo = new osg::Geometry();
      osg::ref_ptr<osg::StateSet> globStateSet = new osg::StateSet();
      osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();

      osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array();
      osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();

      osgGaming::ByteStream stream(bytes);

      int nstars = stream.read<int>();

      for (int i = 0; i < nstars; i++)
      {
        float x = stream.read<float>();
        float y = stream.read<float>();
        float z = stream.read<float>();
        float size = stream.read<float>();

        verts->push_back(osg::Vec3f(x, -z, -y) * 10.0f);
        colors->push_back(osg::Vec4f(size / 8.0f, 0.0f, 0.0f, 1.0f));
      }

      point = new osg::Point();
      osg::ref_ptr<osg::PointSprite> pointSprite = new osg::PointSprite();
      osg::ref_ptr<osg::BlendEquation> blendEquation = new osg::BlendEquation(osg::BlendEquation::FUNC_ADD);

      stateSet->setAttribute(point);
      stateSet->setTextureAttributeAndModes(0, pointSprite, osg::StateAttribute::ON);

      globStateSet->setAttributeAndModes(blendEquation, osg::StateAttribute::ON);
      globStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
      globStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      globStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
      globStateSet->setAttributeAndModes(new osg::BlendFunc(GL_ONE, GL_ONE), osg::StateAttribute::ON);
      stateSet->setRenderBinDetails(-10, "RenderBin");

      // shader
      osg::ref_ptr<osg::Program> pgm = new osg::Program();

      osg::ref_ptr<osg::Shader> vert_shader = resourceManager->loadShader("./GameData/shaders/star.vert", osg::Shader::VERTEX);
      osg::ref_ptr<osg::Shader> frag_shader = resourceManager->loadShader("./GameData/shaders/star.frag", osg::Shader::FRAGMENT);

      pgm->addShader(vert_shader);
      pgm->addShader(frag_shader);

      stateSet->setAttribute(pgm, osg::StateAttribute::ON);
      // ###

      transform->setStateSet(globStateSet);
      geode->setStateSet(stateSet);

      geo->setVertexArray(verts);
      geo->setColorArray(colors);
      geo->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
      geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, verts->size()));

      geode->addDrawable(geo);
      transform->addChild(geode);

      transform->addEventCallback(base);
    }

    void makeSun()
    {
      osg::ref_ptr<osg::Billboard> sunBillboard = new osg::Billboard();
      sunTransform = new osg::PositionAttitudeTransform();
      sunGlowTransform = new osg::PositionAttitudeTransform();
      osg::ref_ptr<osg::PositionAttitudeTransform> sunPosTransform = new osg::PositionAttitudeTransform();
      osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();

      sunBillboard->setMode(osg::Billboard::Mode::POINT_ROT_EYE);
      sunBillboard->setNormal(osg::Vec3(0.0f, -1.0f, 0.0f));

      osg::ref_ptr<osg::Geometry> geo = osgGaming::createQuadGeometry(-1.0f, 1.0f, -1.0f, 1.0f);

      osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
      colors->push_back(osg::Vec4f(1.0, 1.0, 1.0, 1.0));
      geo->setColorArray(colors);
      geo->setColorBinding(osg::Geometry::BIND_OVERALL);

      sunPosTransform->setPosition(osg::Vec3f(0.0f, 9.0f, 0.0f));
      sunPosTransform->setScale(osg::Vec3f(0.3f, 0.3f, 0.3f));

      textureFactory->make()
        ->image(resourceManager->loadImage("./GameData/textures/sun/sprite.png"))
        ->assign(stateSet)
        ->build();

      textureFactory->make()
        ->image(resourceManager->loadImage("./GameData/textures/sun/blend.png"))
        ->assign(sunGlowTransform->getOrCreateStateSet())
        ->build();

      sunBillboard->getOrCreateStateSet()->setRenderBinDetails(-10, "RenderBin");
      sunGlowTransform->getOrCreateStateSet()->setRenderBinDetails(10, "RenderBin");

      sunPosTransform->setStateSet(stateSet);

      transform->addChild(sunTransform);
      sunTransform->addChild(sunPosTransform);
      sunPosTransform->addChild(sunBillboard);
      sunPosTransform->addChild(sunGlowTransform);
      sunGlowTransform->addChild(sunBillboard);
      sunBillboard->addDrawable(geo);
    }

    BackgroundModel* base;

    osg::ref_ptr<osgGaming::ResourceManager> resourceManager;
    osg::ref_ptr<osgGaming::TextureFactory> textureFactory;

    osg::ref_ptr<osg::PositionAttitudeTransform> transform;
    osg::ref_ptr<osg::PositionAttitudeTransform> sunTransform;
    osg::ref_ptr<osg::PositionAttitudeTransform> sunGlowTransform;
    osg::ref_ptr<osg::Point> point;
  };

  BackgroundModel::BackgroundModel(osgGaming::Injector& injector)
    : GUIEventHandler()
    , m(new Impl(injector, this))
  {
    m->transform = new osg::PositionAttitudeTransform();
  }

  BackgroundModel::~BackgroundModel()
  {
  }

  void BackgroundModel::loadStars(std::string filename)
  {
    m->makeStars(filename);
    m->makeSun();
  }

  osg::ref_ptr<osg::PositionAttitudeTransform> BackgroundModel::getTransform()
  {
    return m->transform;
  }

  osg::ref_ptr<osg::PositionAttitudeTransform> BackgroundModel::getSunTransform()
  {
    return m->sunTransform;
  }

  osg::ref_ptr<osg::PositionAttitudeTransform> BackgroundModel::getSunGlowTransform()
  {
    return m->sunGlowTransform;
  }

  void BackgroundModel::updateResolutionHeight(float height)
  {
    m->point->setSize(height / 100.0f);
  }

  bool BackgroundModel::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
  {
    switch (ea.getEventType())
    {
    case osgGA::GUIEventAdapter::RESIZE:

      updateResolutionHeight(ea.getWindowHeight());

      return true;

    default:
      break;

    }

    return false;
  }

}