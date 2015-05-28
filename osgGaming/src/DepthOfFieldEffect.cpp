#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/StaticResources.h>

using namespace osgGaming;
using namespace osg;

DepthOfFieldEffect::DepthOfFieldEffect(float zNear, float zFar)
	: PostProcessingEffect(),
	  _zNear(zNear),
	  _zFar(zFar),
	  _gaussSigma(1.5f),
	  _gaussRadius(5.0f),
	  _focalLength(10.0f),
	  _focalRange(8.0f)
{

}

PostProcessingEffect::InitialUnitList DepthOfFieldEffect::getInitialUnits()
{
	PostProcessingEffect::InitialUnitList list;

	PostProcessingEffect::InitialUnit unitResampleLight;
	unitResampleLight.type = PostProcessingEffect::ONGOING_COLOR;
	unitResampleLight.unit = _unitResampleLight;

	PostProcessingEffect::InitialUnit unitResampleStrong;
	unitResampleStrong.type = PostProcessingEffect::ONGOING_COLOR;
	unitResampleStrong.unit = _unitResampleStrong;

	list.push_back(unitResampleLight);
	list.push_back(unitResampleStrong);

	return list;
}

ref_ptr<osgPPU::Unit> DepthOfFieldEffect::getResultUnit()
{
	return _unitDof;
}

PostProcessingEffect::InputToUniformList DepthOfFieldEffect::getInputToUniform()
{
	PostProcessingEffect::InputToUniformList list;

	InputToUniform ituBypass;
	ituBypass.name = "texColorMap";
	ituBypass.type = PostProcessingEffect::ONGOING_COLOR;
	ituBypass.unit = _unitDof;

	InputToUniform ituDepthBypass;
	ituDepthBypass.name = "texDepthMap";
	ituDepthBypass.type = PostProcessingEffect::BYPASS_DEPTH;
	ituDepthBypass.unit = _unitDof;

	list.push_back(ituBypass);
	list.push_back(ituDepthBypass);

	return list;
}

void DepthOfFieldEffect::setFocalLengthRange(float length, float range)
{
	_focalLength = length;
	_focalRange = range;

	_dofShaderAttribute->set("focalLength", _focalLength);
	_dofShaderAttribute->set("focalRange", _focalRange);
}

void DepthOfFieldEffect::initializeUnits()
{
	ref_ptr<Shader> shaderDepthOfFieldFp = new Shader(Shader::FRAGMENT);
	ref_ptr<Shader> shaderGaussConvolution1dxFp = new Shader(Shader::FRAGMENT);
	ref_ptr<Shader> shaderGaussConvolution1dyFp = new Shader(Shader::FRAGMENT);
	ref_ptr<Shader> shaderGaussConvolutionVp = new Shader(Shader::VERTEX);

	shaderDepthOfFieldFp->setShaderSource(StaticResources::ShaderDepthOfFieldFp);
	shaderGaussConvolution1dxFp->setShaderSource(StaticResources::ShaderGaussConvolution1dxFp);
	shaderGaussConvolution1dyFp->setShaderSource(StaticResources::ShaderGaussConvolution1dyFp);
	shaderGaussConvolutionVp->setShaderSource(StaticResources::ShaderGaussConvolutionVp);


	_unitResampleLight = new osgPPU::UnitInResampleOut();
	{
		_unitResampleLight->setFactorX(0.5);
		_unitResampleLight->setFactorY(0.5);
	}

	osgPPU::ShaderAttribute* gaussx = new osgPPU::ShaderAttribute();
	osgPPU::ShaderAttribute* gaussy = new osgPPU::ShaderAttribute();
	{
		gaussx->addShader(shaderGaussConvolutionVp);
		gaussx->addShader(shaderGaussConvolution1dxFp);

		gaussx->add("sigma", osg::Uniform::FLOAT);
		gaussx->add("radius", osg::Uniform::FLOAT);
		gaussx->add("texUnit0", osg::Uniform::SAMPLER_2D);

		gaussx->set("sigma", _gaussSigma);
		gaussx->set("radius", _gaussRadius);
		gaussx->set("texUnit0", 0);

		gaussy->addShader(shaderGaussConvolutionVp);
		gaussy->addShader(shaderGaussConvolution1dyFp);

		gaussy->add("sigma", osg::Uniform::FLOAT);
		gaussy->add("radius", osg::Uniform::FLOAT);
		gaussy->add("texUnit0", osg::Uniform::SAMPLER_2D);

		gaussy->set("sigma", _gaussSigma);
		gaussy->set("radius", _gaussRadius);
		gaussy->set("texUnit0", 0);
	}

	osgPPU::UnitInOut* blurxlight = new osgPPU::UnitInOut();
	osgPPU::UnitInOut* blurylight = new osgPPU::UnitInOut();
	{
		blurxlight->getOrCreateStateSet()->setAttributeAndModes(gaussx);
		blurylight->getOrCreateStateSet()->setAttributeAndModes(gaussy);
	}
	_unitResampleLight->addChild(blurxlight);
	blurxlight->addChild(blurylight);


	_unitResampleStrong = new osgPPU::UnitInResampleOut();
	{
		_unitResampleStrong->setFactorX(0.25f);
		_unitResampleStrong->setFactorY(0.25f);
	}

	osgPPU::UnitInOut* blurxstrong = new osgPPU::UnitInOut();
	osgPPU::UnitInOut* blurystrong = new osgPPU::UnitInOut();
	{
		blurxstrong->getOrCreateStateSet()->setAttributeAndModes(gaussx);
		blurystrong->getOrCreateStateSet()->setAttributeAndModes(gaussy);
	}
	_unitResampleStrong->addChild(blurxstrong);
	blurxstrong->addChild(blurystrong);

	_unitDof = new osgPPU::UnitInOut();
	{
		_dofShaderAttribute = new osgPPU::ShaderAttribute();
		_dofShaderAttribute->addShader(shaderDepthOfFieldFp);

		_dofShaderAttribute->add("focalLength", osg::Uniform::FLOAT);
		_dofShaderAttribute->add("focalRange", osg::Uniform::FLOAT);
		_dofShaderAttribute->add("zNear", osg::Uniform::FLOAT);
		_dofShaderAttribute->add("zFar", osg::Uniform::FLOAT);

		_dofShaderAttribute->set("focalLength", _focalLength);
		_dofShaderAttribute->set("focalRange", _focalRange);
		_dofShaderAttribute->set("zNear", _zNear);
		_dofShaderAttribute->set("zFar", _zFar);

		_unitDof->getOrCreateStateSet()->setAttributeAndModes(_dofShaderAttribute);
		_unitDof->setInputTextureIndexForViewportReference(0);

		_unitDof->setInputToUniform(blurylight, "texBlurredColorMap", true);
		_unitDof->setInputToUniform(blurystrong, "texStrongBlurredColorMap", true);
	}
}