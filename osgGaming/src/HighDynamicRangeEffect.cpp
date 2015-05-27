#include <osgGaming/HighDynamicRangeEffect.h>
#include <osgGaming/StaticResources.h>

#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>

#include <osgPPU/ShaderAttribute.h>
#include <osgPPU/UnitInMipmapOut.h>

using namespace osgGaming;
using namespace osg;

HighDynamicRangeEffect::HighDynamicRangeEffect()
	: PostProcessingEffect(),
	  _midGrey(5.0f),
	  _hdrBlurSigma(4.0f),
	  _hdrBlurRadius(7.0f),
	  _glareFactor(20.0f),
	  _minLuminance(0.2f),
	  _maxLuminance(5.0f),
	  _adaptFactor(0.01f)
{

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

void HighDynamicRangeEffect::initializeUnits()
{
	ref_ptr<Shader> shaderBrightpassFp = new Shader(Shader::FRAGMENT);
	ref_ptr<Shader> shaderGaussConvolution1dxFp = new Shader(Shader::FRAGMENT);
	ref_ptr<Shader> shaderGaussConvolution1dyFp = new Shader(Shader::FRAGMENT);
	ref_ptr<Shader> shaderGaussConvolutionVp = new Shader(Shader::VERTEX);
	ref_ptr<Shader> shaderLuminanceAdaptedFp = new Shader(Shader::FRAGMENT);
	ref_ptr<Shader> shaderLuminanceFp = new Shader(Shader::FRAGMENT);
	ref_ptr<Shader> shaderLuminanceMipmapFp = new Shader(Shader::FRAGMENT);
	ref_ptr<Shader> shaderTonemapHdrFp = new Shader(Shader::FRAGMENT);

	shaderBrightpassFp->setShaderSource(StaticResources::ShaderBrightpassFp);
	shaderGaussConvolution1dxFp->setShaderSource(StaticResources::ShaderGaussConvolution1dxFp);
	shaderGaussConvolution1dyFp->setShaderSource(StaticResources::ShaderGaussConvolution1dyFp);
	shaderGaussConvolutionVp->setShaderSource(StaticResources::ShaderGaussConvolutionVp);
	shaderLuminanceAdaptedFp->setShaderSource(StaticResources::ShaderLuminanceAdaptedFp);
	shaderLuminanceFp->setShaderSource(StaticResources::ShaderLuminanceFp);
	shaderLuminanceMipmapFp->setShaderSource(StaticResources::ShaderLuminanceMipmapFp);
	shaderTonemapHdrFp->setShaderSource(StaticResources::ShaderTonemapHdrFp);

	_unitResample = new osgPPU::UnitInResampleOut();
	{
		_unitResample->setFactorX(0.25);
		_unitResample->setFactorY(0.25);

		//_unitResample->setFactorX(1.0);
		//_unitResample->setFactorY(1.0);
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
		osgPPU::ShaderAttribute* brightpassSh = new osgPPU::ShaderAttribute();
		brightpassSh->addShader(shaderBrightpassFp);

		brightpassSh->add("g_fMiddleGray", osg::Uniform::FLOAT);
		brightpassSh->set("g_fMiddleGray", _midGrey);
		brightpass->getOrCreateStateSet()->setAttributeAndModes(brightpassSh);

		brightpass->setInputToUniform(_unitResample, "hdrInput", true);
		brightpass->setInputToUniform(sceneLuminance, "lumInput", true);
	}

	osgPPU::UnitInOut* blurx = new osgPPU::UnitInOut();
	osgPPU::UnitInOut* blury = new osgPPU::UnitInOut();
	{
		osgPPU::ShaderAttribute* gaussx = new osgPPU::ShaderAttribute();
		gaussx->addShader(shaderGaussConvolutionVp);
		gaussx->addShader(shaderGaussConvolution1dxFp);
		gaussx->add("sigma", osg::Uniform::FLOAT);
		gaussx->add("radius", osg::Uniform::FLOAT);
		gaussx->add("texUnit0", osg::Uniform::SAMPLER_2D);

		gaussx->set("sigma", _hdrBlurSigma);
		gaussx->set("radius", _hdrBlurRadius);
		gaussx->set("texUnit0", 0);

		blurx->getOrCreateStateSet()->setAttributeAndModes(gaussx);

		osgPPU::ShaderAttribute* gaussy = new osgPPU::ShaderAttribute();
		gaussy->addShader(shaderGaussConvolutionVp);
		gaussy->addShader(shaderGaussConvolution1dyFp);
		gaussy->add("sigma", osg::Uniform::FLOAT);
		gaussy->add("radius", osg::Uniform::FLOAT);
		gaussy->add("texUnit0", osg::Uniform::SAMPLER_2D);

		gaussy->set("sigma", _hdrBlurSigma);
		gaussy->set("radius", _hdrBlurRadius);
		gaussy->set("texUnit0", 0);

		blury->getOrCreateStateSet()->setAttributeAndModes(gaussy);
	}

	brightpass->addChild(blurx);
	blurx->addChild(blury);

	_unitHdr = new osgPPU::UnitInOut();
	{
		osgPPU::ShaderAttribute* sh = new osgPPU::ShaderAttribute();
		sh->addShader(shaderTonemapHdrFp);

		sh->add("fBlurFactor", osg::Uniform::FLOAT);
		sh->add("g_fMiddleGray", osg::Uniform::FLOAT);

		sh->set("fBlurFactor", _glareFactor);
		sh->set("g_fMiddleGray", _midGrey);

		_unitHdr->getOrCreateStateSet()->setAttributeAndModes(sh);
		_unitHdr->setInputTextureIndexForViewportReference(0);

		_unitHdr->setInputToUniform(blury, "blurInput", true);
		_unitHdr->setInputToUniform(sceneLuminance, "lumInput", true);
	}

	_unitAdaptedLuminance = new osgPPU::UnitInOut();
	{
		osgPPU::ShaderAttribute* adaptedShader = new osgPPU::ShaderAttribute();
		adaptedShader->addShader(shaderLuminanceAdaptedFp);
		adaptedShader->add("texLuminance", osg::Uniform::SAMPLER_2D);
		adaptedShader->set("texLuminance", 0);
		adaptedShader->add("texAdaptedLuminance", osg::Uniform::SAMPLER_2D);
		adaptedShader->set("texAdaptedLuminance", 1);

		adaptedShader->add("maxLuminance", osg::Uniform::FLOAT);
		adaptedShader->add("minLuminance", osg::Uniform::FLOAT);
		adaptedShader->add("adaptScaleFactor", osg::Uniform::FLOAT);

		_unitAdaptedLuminance->getOrCreateStateSet()->getOrCreateUniform("invFrameTime", osg::Uniform::FLOAT);

		adaptedShader->set("maxLuminance", _maxLuminance);
		adaptedShader->set("minLuminance", _minLuminance);
		adaptedShader->set("adaptScaleFactor", _adaptFactor);

		_unitAdaptedLuminance->getOrCreateStateSet()->setAttributeAndModes(adaptedShader);
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