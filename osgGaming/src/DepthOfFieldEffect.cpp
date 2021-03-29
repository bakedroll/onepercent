#include <osgGaming/Injector.h>
#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/StaticResources.h>
#include <osgGaming/ShaderFactory.h>

namespace osgGaming
{
  struct DepthOfFieldEffect::Impl
  {
    Impl(Injector& injector)
      : shaderFactory(injector.inject<ShaderFactory>())
      , gaussSigma(1.5f)
      , gaussRadius(5.0f)
      , focalLength(10.0f)
      , focalRange(8.0f)
      , zNear(1.0f)
      , zFar(1000.0f)
    {
      
    }

    osg::ref_ptr<ShaderFactory> shaderFactory;

    float gaussSigma;
    float gaussRadius;
    float focalLength;
    float focalRange;
    float zNear;
    float zFar;

    osg::ref_ptr<osgPPU::ShaderAttribute> shaderDof;
    osg::ref_ptr<osgPPU::ShaderAttribute> shaderGaussX;
    osg::ref_ptr<osgPPU::ShaderAttribute> shaderGaussY;
    osg::ref_ptr<osgPPU::UnitInResampleOut> unitResampleLight;
    osg::ref_ptr<osgPPU::UnitInResampleOut> unitResampleStrong;
    osg::ref_ptr<osgPPU::UnitInOut> unitDof;
  };

  const std::string DepthOfFieldEffect::NAME = "dofEffect";

  DepthOfFieldEffect::DepthOfFieldEffect(Injector& injector)
    : PostProcessingEffect()
    , m(new Impl(injector))
  {

  }

  DepthOfFieldEffect::~DepthOfFieldEffect() = default;

  std::string DepthOfFieldEffect::getName()
  {
    return NAME;
  }

  PostProcessingEffect::InitialUnitList DepthOfFieldEffect::getInitialUnits()
  {
    PostProcessingEffect::InitialUnitList list;

    PostProcessingEffect::InitialUnit unitResampleLight;
    unitResampleLight.type = PostProcessingEffect::UnitType::OngoingColor;
    unitResampleLight.unit = m->unitResampleLight;

    PostProcessingEffect::InitialUnit unitResampleStrong;
    unitResampleStrong.type = PostProcessingEffect::UnitType::OngoingColor;
    unitResampleStrong.unit = m->unitResampleStrong;

    list.push_back(unitResampleLight);
    list.push_back(unitResampleStrong);

    return list;
  }

  osg::ref_ptr<osgPPU::Unit> DepthOfFieldEffect::getResultUnit()
  {
    return m->unitDof;
  }

  PostProcessingEffect::InputToUniformList DepthOfFieldEffect::getInputToUniform()
  {
    PostProcessingEffect::InputToUniformList list;

    InputToUniform ituBypass;
    ituBypass.name = "texColorMap";
    ituBypass.type = PostProcessingEffect::UnitType::OngoingColor;
    ituBypass.unit = m->unitDof;

    InputToUniform ituDepthBypass;
    ituDepthBypass.name = "texDepthMap";
    ituDepthBypass.type = PostProcessingEffect::UnitType::BypassDepth;
    ituDepthBypass.unit = m->unitDof;

    list.push_back(ituBypass);
    list.push_back(ituDepthBypass);

    return list;
  }

  void DepthOfFieldEffect::setGaussSigma(float gaussSigma)
  {
    m->gaussSigma = gaussSigma;

    if (isInitialized())
    {
      m->shaderGaussX->set("sigma", m->gaussSigma);
      m->shaderGaussY->set("sigma", m->gaussSigma);
    }
  }

  void DepthOfFieldEffect::setGaussRadius(float gaussRadius)
  {
    m->gaussRadius = gaussRadius;

    if (isInitialized())
    {
      m->shaderGaussX->set("radius", m->gaussRadius);
      m->shaderGaussY->set("radius", m->gaussRadius);
    }
  }

  void DepthOfFieldEffect::setFocalLength(float focalLength)
  {
    m->focalLength = focalLength;

    if (isInitialized())
    {
      m->shaderDof->set("focalLength", m->focalLength);
    }
  }

  void DepthOfFieldEffect::setFocalRange(float focalRange)
  {
    m->focalRange = focalRange;

    if (isInitialized())
    {
      m->shaderDof->set("focalRange", m->focalRange);
    }
  }

  void DepthOfFieldEffect::setZNear(float zNear)
  {
    m->zNear = zNear;

    if (isInitialized())
    {
      m->shaderDof->set("zNear", m->zNear);
    }
  }

  void DepthOfFieldEffect::setZFar(float zFar)
  {
    m->zFar = zFar;

    if (isInitialized())
    {
      m->shaderDof->set("zFar", m->zFar);
    }
  }

  float DepthOfFieldEffect::getGaussSigma()
  {
    return m->gaussSigma;
  }

  float DepthOfFieldEffect::getGaussRadius()
  {
    return m->gaussRadius;
  }

  float DepthOfFieldEffect::getFocalLength()
  {
    return m->focalLength;
  }

  float DepthOfFieldEffect::getFocalRange()
  {
    return m->focalRange;
  }

  float DepthOfFieldEffect::getZNear()
  {
    return m->zNear;
  }

  float DepthOfFieldEffect::getZFar()
  {
    return m->zFar;
  }

  void DepthOfFieldEffect::initializeUnits()
  {
    osg::ref_ptr<osg::Shader> shaderDepthOfFieldFp = m->shaderFactory->fromSourceText("ShaderDepthOfFieldFp", StaticResources::ShaderDepthOfFieldFp, osg::Shader::FRAGMENT);
    osg::ref_ptr<osg::Shader> shaderGaussConvolution1dxFp = m->shaderFactory->fromSourceText("ShaderGaussConvolution1dxFp", StaticResources::ShaderGaussConvolution1dxFp, osg::Shader::FRAGMENT);
    osg::ref_ptr<osg::Shader> shaderGaussConvolution1dyFp = m->shaderFactory->fromSourceText("ShaderGaussConvolution1dyFp", StaticResources::ShaderGaussConvolution1dyFp, osg::Shader::FRAGMENT);
    osg::ref_ptr<osg::Shader> shaderGaussConvolutionVp = m->shaderFactory->fromSourceText("ShaderGaussConvolutionVp", StaticResources::ShaderGaussConvolutionVp, osg::Shader::VERTEX);

    m->unitResampleLight = new osgPPU::UnitInResampleOut();
    {
      m->unitResampleLight->setFactorX(0.5);
      m->unitResampleLight->setFactorY(0.5);
    }

    m->shaderGaussX = new osgPPU::ShaderAttribute();
    m->shaderGaussY = new osgPPU::ShaderAttribute();
    {
      m->shaderGaussX->addShader(shaderGaussConvolutionVp);
      m->shaderGaussX->addShader(shaderGaussConvolution1dxFp);

      m->shaderGaussX->add("sigma", osg::Uniform::FLOAT);
      m->shaderGaussX->add("radius", osg::Uniform::FLOAT);
      m->shaderGaussX->add("texUnit0", osg::Uniform::SAMPLER_2D);

      m->shaderGaussX->set("sigma", m->gaussSigma);
      m->shaderGaussX->set("radius", m->gaussRadius);
      m->shaderGaussX->set("texUnit0", 0);

      m->shaderGaussY->addShader(shaderGaussConvolutionVp);
      m->shaderGaussY->addShader(shaderGaussConvolution1dyFp);

      m->shaderGaussY->add("sigma", osg::Uniform::FLOAT);
      m->shaderGaussY->add("radius", osg::Uniform::FLOAT);
      m->shaderGaussY->add("texUnit0", osg::Uniform::SAMPLER_2D);

      m->shaderGaussY->set("sigma", m->gaussSigma);
      m->shaderGaussY->set("radius", m->gaussRadius);
      m->shaderGaussY->set("texUnit0", 0);
    }

    osgPPU::UnitInOut* blurxlight = new osgPPU::UnitInOut();
    osgPPU::UnitInOut* blurylight = new osgPPU::UnitInOut();
    {
      blurxlight->getOrCreateStateSet()->setAttributeAndModes(m->shaderGaussX);
      blurylight->getOrCreateStateSet()->setAttributeAndModes(m->shaderGaussY);
    }
    m->unitResampleLight->addChild(blurxlight);
    blurxlight->addChild(blurylight);


    m->unitResampleStrong = new osgPPU::UnitInResampleOut();
    {
      m->unitResampleStrong->setFactorX(0.25f);
      m->unitResampleStrong->setFactorY(0.25f);
    }

    osgPPU::UnitInOut* blurxstrong = new osgPPU::UnitInOut();
    osgPPU::UnitInOut* blurystrong = new osgPPU::UnitInOut();
    {
      blurxstrong->getOrCreateStateSet()->setAttributeAndModes(m->shaderGaussX);
      blurystrong->getOrCreateStateSet()->setAttributeAndModes(m->shaderGaussY);
    }
    m->unitResampleStrong->addChild(blurxstrong);
    blurxstrong->addChild(blurystrong);

    m->unitDof = new osgPPU::UnitInOut();
    {
      m->shaderDof = new osgPPU::ShaderAttribute();
      m->shaderDof->addShader(shaderDepthOfFieldFp);

      m->shaderDof->add("focalLength", osg::Uniform::FLOAT);
      m->shaderDof->add("focalRange", osg::Uniform::FLOAT);
      m->shaderDof->add("zNear", osg::Uniform::FLOAT);
      m->shaderDof->add("zFar", osg::Uniform::FLOAT);

      m->shaderDof->set("focalLength", m->focalLength);
      m->shaderDof->set("focalRange", m->focalRange);
      m->shaderDof->set("zNear", m->zNear);
      m->shaderDof->set("zFar", m->zFar);

      m->unitDof->getOrCreateStateSet()->setAttributeAndModes(m->shaderDof);
      m->unitDof->setInputTextureIndexForViewportReference(0);

      m->unitDof->setInputToUniform(blurylight, "texBlurredColorMap", true);
      m->unitDof->setInputToUniform(blurystrong, "texStrongBlurredColorMap", true);
    }
  }

}