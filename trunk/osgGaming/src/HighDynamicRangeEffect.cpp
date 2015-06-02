#include <osgGaming/HighDynamicRangeEffect.h>
#include <osgGaming/StaticResources.h>
#include <osgGaming/ShaderFactory.h>

#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>

#include <osgPPU/UnitInMipmapOut.h>

using namespace osgGaming;
using namespace osg;
using namespace std;

const string HighDynamicRangeEffect::NAME = "hdrEffect";

HighDynamicRangeEffect::HighDynamicRangeEffect()
	: PostProcessingEffect(),
	  SimulationCallback(),
	  _midGrey(5.0f),
	  _hdrBlurSigma(4.0f),
	  _hdrBlurRadius(7.0f),
	  _glareFactor(20.0f),
	  _minLuminance(0.2f),
	  _maxLuminance(5.0f),
	  _adaptFactor(0.01f)
{

}

string HighDynamicRangeEffect::getName()
{
	return NAME;
}

PostProcessingEffect::InitialUnitList HighDynamicRangeEffect::getInitialUnits()
{
	PostProcessingEffect::InitialUnitList list;
	
	PostProcessingEffect::InitialUnit unitResample;
	unitResample.type = PostProcessingEffect::ONGOING_COLOR;
	unitResample.unit = _unitResample;

	list.push_back(unitResample);

	return list;
}

ref_ptr<osgPPU::Unit> HighDynamicRangeEffect::getResultUnit()
{
	return _unitHdr;
}

PostProcessingEffect::InputToUniformList HighDynamicRangeEffect::getInputToUniform()
{
	PostProcessingEffect::InputToUniformList list;

	InputToUniform ituBypass;
	ituBypass.name = "hdrInput";
	ituBypass.type = PostProcessingEffect::ONGOING_COLOR;
	ituBypass.unit = _unitHdr;

	list.push_back(ituBypass);

	return list;
}

void HighDynamicRangeEffect::action(Node* node, NodeVisitor* nv, double simTime, double timeDiff)
{
	_unitAdaptedLuminance->getOrCreateStateSet()->getOrCreateUniform("invFrameTime", osg::Uniform::FLOAT)->set(float(timeDiff));
}

void HighDynamicRangeEffect::setMidGrey(float midGrey)
{
	_midGrey = midGrey;
	
	if (isInitialized())
	{
		_shaderBrightpass->set("g_fMiddleGray", _midGrey);
		_shaderHdr->set("g_fMiddleGray", _midGrey);
	}

	printf("Set midGrey to %f\n", _midGrey);
}

void HighDynamicRangeEffect::setBlurSigma(float blurSigma)
{
	_hdrBlurSigma = blurSigma;

	if (isInitialized())
	{
		_shaderGaussX->set("sigma", _hdrBlurSigma);
		_shaderGaussY->set("sigma", _hdrBlurSigma);
	}

	printf("Set blurSigma to %f\n", _hdrBlurSigma);
}

void HighDynamicRangeEffect::setBlurRadius(float blurRadius)
{
	_hdrBlurRadius = blurRadius;

	if (isInitialized())
	{
		_shaderGaussX->set("radius", _hdrBlurRadius);
		_shaderGaussY->set("radius", _hdrBlurRadius);
	}

	printf("Set blurRadius to %f\n", _hdrBlurRadius);
}

void HighDynamicRangeEffect::setGlareFactor(float glareFactor)
{
	_glareFactor = glareFactor;

	if (isInitialized())
	{
		_shaderHdr->set("fBlurFactor", _glareFactor);
	}

	printf("Set glateFactor to %f\n", _glareFactor);
}

void HighDynamicRangeEffect::setAdaptFactor(float adaptFactor)
{
	_adaptFactor = adaptFactor;

	if (isInitialized())
	{
		_shaderAdapted->set("adaptScaleFactor", _adaptFactor);
	}

	printf("Set adaptedFactor to %f\n", _adaptFactor);
}

void HighDynamicRangeEffect::setMinLuminance(float minLuminance)
{
	_minLuminance = minLuminance;

	if (isInitialized())
	{
		_shaderAdapted->set("minLuminance", _minLuminance);
	}

	printf("Set minLuminance to %f\n", _minLuminance);
}

void HighDynamicRangeEffect::setMaxLuminance(float maxLuminance)
{
	_maxLuminance = maxLuminance;

	if (isInitialized())
	{
		_shaderAdapted->set("maxLuminance", _maxLuminance);
	}

	printf("Set maxLuminance to %f\n", _maxLuminance);
}

float HighDynamicRangeEffect::getMidGrey()
{
	return _midGrey;
}

float HighDynamicRangeEffect::getBlurSigma()
{
	return _hdrBlurSigma;
}

float HighDynamicRangeEffect::getBlurRadius()
{
	return _hdrBlurRadius;
}

float HighDynamicRangeEffect::getGlareFactor()
{
	return _glareFactor;
}

float HighDynamicRangeEffect::getAdaptFactor()
{
	return _adaptFactor;
}

float HighDynamicRangeEffect::getMinLuminance()
{
	return _minLuminance;
}

float HighDynamicRangeEffect::getMaxLuminance()
{
	return _maxLuminance;
}

void HighDynamicRangeEffect::initializeUnits()
{
	ref_ptr<Shader> shaderBrightpassFp			= ShaderFactory::fromSourceText("ShaderBrightpassFp", StaticResources::ShaderBrightpassFp, Shader::FRAGMENT);
	ref_ptr<Shader> shaderGaussConvolution1dxFp = ShaderFactory::fromSourceText("ShaderGaussConvolution1dxFp", StaticResources::ShaderGaussConvolution1dxFp, Shader::FRAGMENT);
	ref_ptr<Shader> shaderGaussConvolution1dyFp = ShaderFactory::fromSourceText("ShaderGaussConvolution1dyFp", StaticResources::ShaderGaussConvolution1dyFp, Shader::FRAGMENT);
	ref_ptr<Shader> shaderGaussConvolutionVp	= ShaderFactory::fromSourceText("ShaderGaussConvolutionVp", StaticResources::ShaderGaussConvolutionVp, Shader::VERTEX);
	ref_ptr<Shader> shaderLuminanceAdaptedFp	= ShaderFactory::fromSourceText("ShaderLuminanceAdaptedFp", StaticResources::ShaderLuminanceAdaptedFp, Shader::FRAGMENT);
	ref_ptr<Shader> shaderLuminanceFp			= ShaderFactory::fromSourceText("ShaderLuminanceFp", StaticResources::ShaderLuminanceFp, Shader::FRAGMENT);
	ref_ptr<Shader> shaderLuminanceMipmapFp		= ShaderFactory::fromSourceText("ShaderLuminanceMipmapFp", StaticResources::ShaderLuminanceMipmapFp, Shader::FRAGMENT);
	ref_ptr<Shader> shaderTonemapHdrFp			= ShaderFactory::fromSourceText("ShaderTonemapHdrFp", StaticResources::ShaderTonemapHdrFp, Shader::FRAGMENT);

	_unitResample = new osgPPU::UnitInResampleOut();
	{
		_unitResample->setFactorX(0.25);
		_unitResample->setFactorY(0.25);
	}

	osgPPU::UnitInOut* pixelLuminance = new osgPPU::UnitInOut();
	{
		osgPPU::ShaderAttribute* lumShader = new osgPPU::ShaderAttribute();
		lumShader->addShader(shaderLuminanceFp);
		lumShader->add("texUnit0", osg::Uniform::SAMPLER_2D);
		lumShader->set("texUnit0", 0);

		pixelLuminance->getOrCreateStateSet()->setAttributeAndModes(lumShader);
	}
	_unitResample->addChild(pixelLuminance);

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
		_shaderBrightpass = new osgPPU::ShaderAttribute();
		_shaderBrightpass->addShader(shaderBrightpassFp);

		_shaderBrightpass->add("g_fMiddleGray", osg::Uniform::FLOAT);
		_shaderBrightpass->set("g_fMiddleGray", _midGrey);
		brightpass->getOrCreateStateSet()->setAttributeAndModes(_shaderBrightpass);

		brightpass->setInputToUniform(_unitResample, "hdrInput", true);
		brightpass->setInputToUniform(sceneLuminance, "lumInput", true);
	}

	osgPPU::UnitInOut* blurx = new osgPPU::UnitInOut();
	osgPPU::UnitInOut* blury = new osgPPU::UnitInOut();
	{
		_shaderGaussX = new osgPPU::ShaderAttribute();
		_shaderGaussX->addShader(shaderGaussConvolutionVp);
		_shaderGaussX->addShader(shaderGaussConvolution1dxFp);
		_shaderGaussX->add("sigma", osg::Uniform::FLOAT);
		_shaderGaussX->add("radius", osg::Uniform::FLOAT);
		_shaderGaussX->add("texUnit0", osg::Uniform::SAMPLER_2D);

		_shaderGaussX->set("sigma", _hdrBlurSigma);
		_shaderGaussX->set("radius", _hdrBlurRadius);
		_shaderGaussX->set("texUnit0", 0);

		blurx->getOrCreateStateSet()->setAttributeAndModes(_shaderGaussX);

		_shaderGaussY = new osgPPU::ShaderAttribute();
		_shaderGaussY->addShader(shaderGaussConvolutionVp);
		_shaderGaussY->addShader(shaderGaussConvolution1dyFp);
		_shaderGaussY->add("sigma", osg::Uniform::FLOAT);
		_shaderGaussY->add("radius", osg::Uniform::FLOAT);
		_shaderGaussY->add("texUnit0", osg::Uniform::SAMPLER_2D);

		_shaderGaussY->set("sigma", _hdrBlurSigma);
		_shaderGaussY->set("radius", _hdrBlurRadius);
		_shaderGaussY->set("texUnit0", 0);

		blury->getOrCreateStateSet()->setAttributeAndModes(_shaderGaussY);
	}

	brightpass->addChild(blurx);
	blurx->addChild(blury);

	_unitHdr = new osgPPU::UnitInOut();
	{
		_shaderHdr = new osgPPU::ShaderAttribute();
		_shaderHdr->addShader(shaderTonemapHdrFp);

		_shaderHdr->add("fBlurFactor", osg::Uniform::FLOAT);
		_shaderHdr->add("g_fMiddleGray", osg::Uniform::FLOAT);

		_shaderHdr->set("fBlurFactor", _glareFactor);
		_shaderHdr->set("g_fMiddleGray", _midGrey);

		_unitHdr->getOrCreateStateSet()->setAttributeAndModes(_shaderHdr);
		_unitHdr->setInputTextureIndexForViewportReference(-1);

		_unitHdr->setInputToUniform(blury, "blurInput", true);
		_unitHdr->setInputToUniform(sceneLuminance, "lumInput", true);
	}

	_unitAdaptedLuminance = new osgPPU::UnitInOut();
	{
		_shaderAdapted = new osgPPU::ShaderAttribute();
		_shaderAdapted->addShader(shaderLuminanceAdaptedFp);
		_shaderAdapted->add("texLuminance", osg::Uniform::SAMPLER_2D);
		_shaderAdapted->set("texLuminance", 0);
		_shaderAdapted->add("texAdaptedLuminance", osg::Uniform::SAMPLER_2D);
		_shaderAdapted->set("texAdaptedLuminance", 1);

		_shaderAdapted->add("maxLuminance", osg::Uniform::FLOAT);
		_shaderAdapted->add("minLuminance", osg::Uniform::FLOAT);
		_shaderAdapted->add("adaptScaleFactor", osg::Uniform::FLOAT);

		_unitAdaptedLuminance->getOrCreateStateSet()->getOrCreateUniform("invFrameTime", osg::Uniform::FLOAT);

		_shaderAdapted->set("maxLuminance", _maxLuminance);
		_shaderAdapted->set("minLuminance", _minLuminance);
		_shaderAdapted->set("adaptScaleFactor", _adaptFactor);

		_unitAdaptedLuminance->getOrCreateStateSet()->setAttributeAndModes(_shaderAdapted);
		_unitAdaptedLuminance->setViewport(new osg::Viewport(0, 0, 1, 1));
		_unitAdaptedLuminance->setInputTextureIndexForViewportReference(-1);
	}

	sceneLuminance->addChild(_unitAdaptedLuminance);

	osgPPU::UnitInOut* adaptedlumCopy = new osgPPU::UnitInOut();
	adaptedlumCopy->addChild(_unitAdaptedLuminance);

	_unitAdaptedLuminance->addChild(adaptedlumCopy);
	_unitAdaptedLuminance->addChild(brightpass);
	brightpass->setInputToUniform(_unitAdaptedLuminance, "texAdaptedLuminance");

	_unitAdaptedLuminance->addChild(_unitHdr);
	_unitHdr->setInputToUniform(_unitAdaptedLuminance, "texAdaptedLuminance");

	_unitAdaptedLuminance->setUpdateCallback(this);
}