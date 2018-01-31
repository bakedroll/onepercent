#include <osgGaming/Injector.h>

#include <osgGaming/HighDynamicRangeEffect.h>
#include <osgGaming/StaticResources.h>
#include <osgGaming/ShaderFactory.h>

#include <osgDB/ReadFile>

#include <osgPPU/UnitInMipmapOut.h>
#include <osgPPU/ShaderAttribute.h>
#include <osgPPU/UnitInResampleOut.h>

namespace osgGaming
{

  class HighDynamicRangeEffectCallback : public SimulationCallback
  {
  public:
    HighDynamicRangeEffectCallback(osg::ref_ptr<osgPPU::UnitInOut> unitAdaptedLuminance)
      : SimulationCallback(),
      unitAdaptedLuminance(unitAdaptedLuminance)
    {

    }

    virtual void action(osg::Node* node, osg::NodeVisitor* nv, double simTime, double timeDiff) override
    {
      unitAdaptedLuminance->getOrCreateStateSet()->getOrCreateUniform("invFrameTime", osg::Uniform::FLOAT)->set(float(timeDiff));
    }

  private:
    osg::ref_ptr<osgPPU::UnitInOut> unitAdaptedLuminance;

  };

  struct HighDynamicRangeEffect::Impl
  {
    Impl(Injector& injector)
      : shaderFactory(injector.inject<ShaderFactory>())
      , midGrey(5.0f)
      , hdrBlurSigma(4.0f)
      , hdrBlurRadius(5.0f)
      , glareFactor(7.5f)
      , adaptFactor(0.03f)
      , minLuminance(0.2f)
      , maxLuminance(5.0f)
    {
      
    }

    osg::ref_ptr<ShaderFactory> shaderFactory;

    float midGrey;
    float hdrBlurSigma;
    float hdrBlurRadius;
    float glareFactor;
    float adaptFactor;
    float minLuminance;
    float maxLuminance;

    osg::ref_ptr<osgPPU::UnitInResampleOut> unitResample;
    osg::ref_ptr<osgPPU::UnitInOut> unitHdr;

    osg::ref_ptr<osgPPU::ShaderAttribute> shaderBrightpass;
    osg::ref_ptr<osgPPU::ShaderAttribute> shaderHdr;
    osg::ref_ptr<osgPPU::ShaderAttribute> shaderGaussX;
    osg::ref_ptr<osgPPU::ShaderAttribute> shaderGaussY;
    osg::ref_ptr<osgPPU::ShaderAttribute> shaderAdapted;
  };

  const std::string HighDynamicRangeEffect::NAME = "hdrEffect";

  HighDynamicRangeEffect::HighDynamicRangeEffect(Injector& injector)
    : PostProcessingEffect()
    , m(new Impl(injector))
  {

  }

  HighDynamicRangeEffect::~HighDynamicRangeEffect()
  {
  }

  std::string HighDynamicRangeEffect::getName()
  {
    return NAME;
  }

  PostProcessingEffect::InitialUnitList HighDynamicRangeEffect::getInitialUnits()
  {
    PostProcessingEffect::InitialUnitList list;

    PostProcessingEffect::InitialUnit unitResample;
    unitResample.type = PostProcessingEffect::ONGOING_COLOR;
    unitResample.unit = m->unitResample;

    list.push_back(unitResample);

    return list;
  }

  osg::ref_ptr<osgPPU::Unit> HighDynamicRangeEffect::getResultUnit()
  {
    return m->unitHdr;
  }

  PostProcessingEffect::InputToUniformList HighDynamicRangeEffect::getInputToUniform()
  {
    PostProcessingEffect::InputToUniformList list;

    InputToUniform ituBypass;
    ituBypass.name = "hdrInput";
    ituBypass.type = PostProcessingEffect::ONGOING_COLOR;
    ituBypass.unit = m->unitHdr;

    list.push_back(ituBypass);

    return list;
  }

  void HighDynamicRangeEffect::setMidGrey(float midGrey)
  {
    m->midGrey = midGrey;

    if (isInitialized())
    {
      m->shaderBrightpass->set("g_fMiddleGray", m->midGrey);
      m->shaderHdr->set("g_fMiddleGray", m->midGrey);
    }

    printf("Set midGrey to %f\n", m->midGrey);
  }

  void HighDynamicRangeEffect::setBlurSigma(float blurSigma)
  {
    m->hdrBlurSigma = blurSigma;

    if (isInitialized())
    {
      m->shaderGaussX->set("sigma", m->hdrBlurSigma);
      m->shaderGaussY->set("sigma", m->hdrBlurSigma);
    }

    printf("Set blurSigma to %f\n", m->hdrBlurSigma);
  }

  void HighDynamicRangeEffect::setBlurRadius(float blurRadius)
  {
    m->hdrBlurRadius = blurRadius;

    if (isInitialized())
    {
      m->shaderGaussX->set("radius", m->hdrBlurRadius);
      m->shaderGaussY->set("radius", m->hdrBlurRadius);
    }

    printf("Set blurRadius to %f\n", m->hdrBlurRadius);
  }

  void HighDynamicRangeEffect::setGlareFactor(float glareFactor)
  {
    m->glareFactor = glareFactor;

    if (isInitialized())
    {
      m->shaderHdr->set("fBlurFactor", m->glareFactor);
    }

    printf("Set glateFactor to %f\n", m->glareFactor);
  }

  void HighDynamicRangeEffect::setAdaptFactor(float adaptFactor)
  {
    m->adaptFactor = adaptFactor;

    if (isInitialized())
    {
      m->shaderAdapted->set("adaptScaleFactor", m->adaptFactor);
    }

    printf("Set adaptedFactor to %f\n", m->adaptFactor);
  }

  void HighDynamicRangeEffect::setMinLuminance(float minLuminance)
  {
    m->minLuminance = minLuminance;

    if (isInitialized())
    {
      m->shaderAdapted->set("minLuminance", m->minLuminance);
    }

    printf("Set minLuminance to %f\n", m->minLuminance);
  }

  void HighDynamicRangeEffect::setMaxLuminance(float maxLuminance)
  {
    m->maxLuminance = maxLuminance;

    if (isInitialized())
    {
      m->shaderAdapted->set("maxLuminance", m->maxLuminance);
    }

    printf("Set maxLuminance to %f\n", m->maxLuminance);
  }

  float HighDynamicRangeEffect::getMidGrey()
  {
    return m->midGrey;
  }

  float HighDynamicRangeEffect::getBlurSigma()
  {
    return m->hdrBlurSigma;
  }

  float HighDynamicRangeEffect::getBlurRadius()
  {
    return m->hdrBlurRadius;
  }

  float HighDynamicRangeEffect::getGlareFactor()
  {
    return m->glareFactor;
  }

  float HighDynamicRangeEffect::getAdaptFactor()
  {
    return m->adaptFactor;
  }

  float HighDynamicRangeEffect::getMinLuminance()
  {
    return m->minLuminance;
  }

  float HighDynamicRangeEffect::getMaxLuminance()
  {
    return m->maxLuminance;
  }

  void HighDynamicRangeEffect::initializeUnits()
  {
    osg::ref_ptr<osg::Shader> shaderBrightpassFp = m->shaderFactory->fromSourceText("ShaderBrightpassFp", StaticResources::ShaderBrightpassFp, osg::Shader::FRAGMENT);
    osg::ref_ptr<osg::Shader> shaderGaussConvolution1dxFp = m->shaderFactory->fromSourceText("ShaderGaussConvolution1dxFp", StaticResources::ShaderGaussConvolution1dxFp, osg::Shader::FRAGMENT);
    osg::ref_ptr<osg::Shader> shaderGaussConvolution1dyFp = m->shaderFactory->fromSourceText("ShaderGaussConvolution1dyFp", StaticResources::ShaderGaussConvolution1dyFp, osg::Shader::FRAGMENT);
    osg::ref_ptr<osg::Shader> shaderGaussConvolutionVp = m->shaderFactory->fromSourceText("ShaderGaussConvolutionVp", StaticResources::ShaderGaussConvolutionVp, osg::Shader::VERTEX);
    osg::ref_ptr<osg::Shader> shaderLuminanceAdaptedFp = m->shaderFactory->fromSourceText("ShaderLuminanceAdaptedFp", StaticResources::ShaderLuminanceAdaptedFp, osg::Shader::FRAGMENT);
    osg::ref_ptr<osg::Shader> shaderLuminanceFp = m->shaderFactory->fromSourceText("ShaderLuminanceFp", StaticResources::ShaderLuminanceFp, osg::Shader::FRAGMENT);
    osg::ref_ptr<osg::Shader> shaderLuminanceMipmapFp = m->shaderFactory->fromSourceText("ShaderLuminanceMipmapFp", StaticResources::ShaderLuminanceMipmapFp, osg::Shader::FRAGMENT);
    osg::ref_ptr<osg::Shader> shaderTonemapHdrFp = m->shaderFactory->fromSourceText("ShaderTonemapHdrFp", StaticResources::ShaderTonemapHdrFp, osg::Shader::FRAGMENT);

    m->unitResample = new osgPPU::UnitInResampleOut();
    {
      m->unitResample->setFactorX(0.25);
      m->unitResample->setFactorY(0.25);
    }

    osgPPU::UnitInOut* pixelLuminance = new osgPPU::UnitInOut();
    {
      osgPPU::ShaderAttribute* lumShader = new osgPPU::ShaderAttribute();
      lumShader->addShader(shaderLuminanceFp);
      lumShader->add("texUnit0", osg::Uniform::SAMPLER_2D);
      lumShader->set("texUnit0", 0);

      pixelLuminance->getOrCreateStateSet()->setAttributeAndModes(lumShader);
    }
    m->unitResample->addChild(pixelLuminance);

    osgPPU::UnitInMipmapOut* sceneLuminance = new osgPPU::UnitInMipmapOut();
    {
      osgPPU::ShaderAttribute* lumShaderMipmap = new osgPPU::ShaderAttribute();
      lumShaderMipmap->addShader(shaderLuminanceMipmapFp);

      lumShaderMipmap->add("texUnit0", osg::Uniform::SAMPLER_2D);
      lumShaderMipmap->set("texUnit0", 0);

      sceneLuminance->getOrCreateStateSet()->setAttributeAndModes(lumShaderMipmap);
      sceneLuminance->setGenerateMipmapForInputTexture(0);
    }
    pixelLuminance->addChild(sceneLuminance);

    osgPPU::Unit* brightpass = new osgPPU::UnitInOut();
    {
      m->shaderBrightpass = new osgPPU::ShaderAttribute();
      m->shaderBrightpass->addShader(shaderBrightpassFp);

      m->shaderBrightpass->add("g_fMiddleGray", osg::Uniform::FLOAT);
      m->shaderBrightpass->set("g_fMiddleGray", m->midGrey);
      brightpass->getOrCreateStateSet()->setAttributeAndModes(m->shaderBrightpass);

      brightpass->setInputToUniform(m->unitResample, "hdrInput", true);
      brightpass->setInputToUniform(sceneLuminance, "lumInput", true);
    }

    osgPPU::UnitInOut* blurx = new osgPPU::UnitInOut();
    osgPPU::UnitInOut* blury = new osgPPU::UnitInOut();
    {
      m->shaderGaussX = new osgPPU::ShaderAttribute();
      m->shaderGaussX->addShader(shaderGaussConvolutionVp);
      m->shaderGaussX->addShader(shaderGaussConvolution1dxFp);
      m->shaderGaussX->add("sigma", osg::Uniform::FLOAT);
      m->shaderGaussX->add("radius", osg::Uniform::FLOAT);
      m->shaderGaussX->add("texUnit0", osg::Uniform::SAMPLER_2D);

      m->shaderGaussX->set("sigma", m->hdrBlurSigma);
      m->shaderGaussX->set("radius", m->hdrBlurRadius);
      m->shaderGaussX->set("texUnit0", 0);

      blurx->getOrCreateStateSet()->setAttributeAndModes(m->shaderGaussX);

      m->shaderGaussY = new osgPPU::ShaderAttribute();
      m->shaderGaussY->addShader(shaderGaussConvolutionVp);
      m->shaderGaussY->addShader(shaderGaussConvolution1dyFp);
      m->shaderGaussY->add("sigma", osg::Uniform::FLOAT);
      m->shaderGaussY->add("radius", osg::Uniform::FLOAT);
      m->shaderGaussY->add("texUnit0", osg::Uniform::SAMPLER_2D);

      m->shaderGaussY->set("sigma", m->hdrBlurSigma);
      m->shaderGaussY->set("radius", m->hdrBlurRadius);
      m->shaderGaussY->set("texUnit0", 0);

      blury->getOrCreateStateSet()->setAttributeAndModes(m->shaderGaussY);
    }

    brightpass->addChild(blurx);
    blurx->addChild(blury);

    m->unitHdr = new osgPPU::UnitInOut();
    {
      m->shaderHdr = new osgPPU::ShaderAttribute();
      m->shaderHdr->addShader(shaderTonemapHdrFp);

      m->shaderHdr->add("fBlurFactor", osg::Uniform::FLOAT);
      m->shaderHdr->add("g_fMiddleGray", osg::Uniform::FLOAT);

      m->shaderHdr->set("fBlurFactor", m->glareFactor);
      m->shaderHdr->set("g_fMiddleGray", m->midGrey);

      m->unitHdr->getOrCreateStateSet()->setAttributeAndModes(m->shaderHdr);
      m->unitHdr->setInputTextureIndexForViewportReference(-1);

      m->unitHdr->setInputToUniform(blury, "blurInput", true);
      m->unitHdr->setInputToUniform(sceneLuminance, "lumInput", true);
    }

    osgPPU::UnitInOut* adaptedLuminance = new osgPPU::UnitInOut();
    {
      m->shaderAdapted = new osgPPU::ShaderAttribute();
      m->shaderAdapted->addShader(shaderLuminanceAdaptedFp);
      m->shaderAdapted->add("texLuminance", osg::Uniform::SAMPLER_2D);
      m->shaderAdapted->set("texLuminance", 0);
      m->shaderAdapted->add("texAdaptedLuminance", osg::Uniform::SAMPLER_2D);
      m->shaderAdapted->set("texAdaptedLuminance", 1);

      m->shaderAdapted->add("maxLuminance", osg::Uniform::FLOAT);
      m->shaderAdapted->add("minLuminance", osg::Uniform::FLOAT);
      m->shaderAdapted->add("adaptScaleFactor", osg::Uniform::FLOAT);

      adaptedLuminance->getOrCreateStateSet()->getOrCreateUniform("invFrameTime", osg::Uniform::FLOAT);

      m->shaderAdapted->set("maxLuminance", m->maxLuminance);
      m->shaderAdapted->set("minLuminance", m->minLuminance);
      m->shaderAdapted->set("adaptScaleFactor", m->adaptFactor);

      adaptedLuminance->getOrCreateStateSet()->setAttributeAndModes(m->shaderAdapted);
      adaptedLuminance->setViewport(new osg::Viewport(0, 0, 1, 1));
      adaptedLuminance->setInputTextureIndexForViewportReference(-1);
    }

    sceneLuminance->addChild(adaptedLuminance);

    osgPPU::UnitInOut* adaptedlumCopy = new osgPPU::UnitInOut();
    adaptedlumCopy->addChild(adaptedLuminance);

    adaptedLuminance->addChild(adaptedlumCopy);
    adaptedLuminance->addChild(brightpass);
    brightpass->setInputToUniform(adaptedLuminance, "texAdaptedLuminance");

    adaptedLuminance->addChild(m->unitHdr);
    m->unitHdr->setInputToUniform(adaptedLuminance, "texAdaptedLuminance");

    adaptedLuminance->setUpdateCallback(new HighDynamicRangeEffectCallback(adaptedLuminance));
  }

}