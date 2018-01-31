#include <osgGaming/Injector.h>

#include <osgGaming/FastApproximateAntiAliasingEffect.h>
#include <osgGaming/ShaderFactory.h>
#include <osgGaming/StaticResources.h>

namespace osgGaming
{
  class FastApproximateAntiAliasingEffectCallback : public osgGA::GUIEventHandler
  {
  public:
    FastApproximateAntiAliasingEffectCallback(osg::ref_ptr<osgPPU::ShaderAttribute> shaderFxaa, osg::Vec2f resolution)
      : osgGA::GUIEventHandler(),
      shaderFxaa(shaderFxaa),
      resolution(resolution)
    {

    }

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override
    {
      switch (ea.getEventType())
      {
      case osgGA::GUIEventAdapter::RESIZE:

        int width = ea.getWindowWidth();
        int height = ea.getWindowHeight();

        if (width != int(resolution.x()) || height != int(resolution.y()))
        {
          resolution = osg::Vec2f(float(width), float(height));

          shaderFxaa->set("rt_w", resolution.x());
          shaderFxaa->set("rt_h", resolution.y());

          return true;
        }
      }

      return false;
    }

  private:
    osg::ref_ptr<osgPPU::ShaderAttribute> shaderFxaa;
    osg::Vec2f resolution;
  };

  struct FastApproximateAntiAliasingEffect::Impl
  {
    Impl(Injector& injector)
      : shaderFactory(injector.inject<ShaderFactory>())
      , resolution(osg::Vec2f(512.0f, 512.0f))
    {
    }

    osg::ref_ptr<ShaderFactory> shaderFactory;

    osg::ref_ptr<osgPPU::UnitInOut> unitFxaa;
    osg::Vec2f resolution;
    osg::ref_ptr<osgPPU::ShaderAttribute> shaderFxaa;
  };

  const std::string FastApproximateAntiAliasingEffect::NAME = "fxaaEffect";

  FastApproximateAntiAliasingEffect::FastApproximateAntiAliasingEffect(Injector& injector)
    : PostProcessingEffect()
    , m(new Impl(injector))
  {

  }

  FastApproximateAntiAliasingEffect::~FastApproximateAntiAliasingEffect()
  {
  }

  std::string FastApproximateAntiAliasingEffect::getName()
  {
    return NAME;
  }

  FastApproximateAntiAliasingEffect::InitialUnitList FastApproximateAntiAliasingEffect::getInitialUnits()
  {
    PostProcessingEffect::InitialUnitList list;

    return list;
  }

  osg::ref_ptr<osgPPU::Unit> FastApproximateAntiAliasingEffect::getResultUnit()
  {
    return m->unitFxaa;
  }

  FastApproximateAntiAliasingEffect::InputToUniformList FastApproximateAntiAliasingEffect::getInputToUniform()
  {
    PostProcessingEffect::InputToUniformList list;

    InputToUniform ituBypass;
    ituBypass.name = "tex0";
    ituBypass.type = PostProcessingEffect::ONGOING_COLOR;
    ituBypass.unit = m->unitFxaa;

    list.push_back(ituBypass);

    return list;
  }

  void FastApproximateAntiAliasingEffect::setResolution(osg::Vec2f resolution)
  {
    m->resolution = resolution;

    if (m->shaderFxaa.valid())
    {
      m->shaderFxaa->set("rt_w", m->resolution.x());
      m->shaderFxaa->set("rt_h", m->resolution.y());
    }
  }

  void FastApproximateAntiAliasingEffect::initializeUnits()
  {
    osg::ref_ptr<osg::Shader> shaderFxaaFp = m->shaderFactory->fromSourceText("ShaderFxaaFp", StaticResources::ShaderFxaaFp, osg::Shader::FRAGMENT);
    osg::ref_ptr<osg::Shader> shaderFxaaVp = m->shaderFactory->fromSourceText("ShaderFxaaVp", StaticResources::ShaderFxaaVp, osg::Shader::VERTEX);

    m->unitFxaa = new osgPPU::UnitInOut();
    {
      m->shaderFxaa = new osgPPU::ShaderAttribute();
      m->shaderFxaa->addShader(shaderFxaaFp);
      m->shaderFxaa->addShader(shaderFxaaVp);

      m->shaderFxaa->add("rt_w", osg::Uniform::FLOAT);
      m->shaderFxaa->add("rt_h", osg::Uniform::FLOAT);

      m->shaderFxaa->set("rt_w", m->resolution.x());
      m->shaderFxaa->set("rt_h", m->resolution.y());
      
      m->unitFxaa->getOrCreateStateSet()->setAttributeAndModes(m->shaderFxaa);

      m->unitFxaa->setEventCallback(new FastApproximateAntiAliasingEffectCallback(m->shaderFxaa, m->resolution));
    }
  }

}