#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/StaticResources.h>
#include <osgGaming/ShaderFactory.h>

using namespace osgGaming;
using namespace osg;
using namespace std;

const string DepthOfFieldEffect::NAME = "dofEffect";

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

string DepthOfFieldEffect::getName()
{
	return NAME;
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

void DepthOfFieldEffect::setGaussSigma(float gaussSigma)
{
	_gaussSigma = gaussSigma;

	if (isInitialized())
	{
		_shaderGaussX->set("sigma", _gaussSigma);
		_shaderGaussY->set("sigma", _gaussSigma);
	}
}

void DepthOfFieldEffect::setGaussRadius(float gaussRadius)
{
	_gaussRadius = gaussRadius;

	if (isInitialized())
	{
		_shaderGaussX->set("radius", _gaussRadius);
		_shaderGaussY->set("radius", _gaussRadius);
	}
}

void DepthOfFieldEffect::setFocalLength(float focalLength)
{
	_focalLength = focalLength;

	if (isInitialized())
	{
		_shaderDof->set("focalLength", _focalLength);
	}
}

void DepthOfFieldEffect::setFocalRange(float focalRange)
{
	_focalRange = focalRange;

	if (isInitialized())
	{
		_shaderDof->set("focalRange", _focalRange);
	}
}

void DepthOfFieldEffect::setZNear(float zNear)
{
	_zNear = zNear;

	if (isInitialized())
	{
		_shaderDof->set("zNear", _zNear);
	}
}

void DepthOfFieldEffect::setZFar(float zFar)
{
	_zFar = zFar;

	if (isInitialized())
	{
		_shaderDof->set("zFar", _zFar);
	}
}

float DepthOfFieldEffect::getGaussSigma()
{
	return _gaussSigma;
}

float DepthOfFieldEffect::getGaussRadius()
{
	return _gaussRadius;
}

float DepthOfFieldEffect::getFocalLength()
{
	return _focalLength;
}

float DepthOfFieldEffect::getFocalRange()
{
	return _focalRange;
}

float DepthOfFieldEffect::getZNear()
{
	return _zNear;
}

float DepthOfFieldEffect::getZFar()
{
	return _zFar;
}

void DepthOfFieldEffect::initializeUnits()
{
	ref_ptr<Shader> shaderDepthOfFieldFp		= ShaderFactory::fromSourceText("ShaderDepthOfFieldFp", StaticResources::ShaderDepthOfFieldFp, Shader::FRAGMENT);
	ref_ptr<Shader> shaderGaussConvolution1dxFp = ShaderFactory::fromSourceText("ShaderGaussConvolution1dxFp", StaticResources::ShaderGaussConvolution1dxFp, Shader::FRAGMENT);
	ref_ptr<Shader> shaderGaussConvolution1dyFp = ShaderFactory::fromSourceText("ShaderGaussConvolution1dyFp", StaticResources::ShaderGaussConvolution1dyFp, Shader::FRAGMENT);
	ref_ptr<Shader> shaderGaussConvolutionVp	= ShaderFactory::fromSourceText("ShaderGaussConvolutionVp", StaticResources::ShaderGaussConvolutionVp, Shader::VERTEX);

	_unitResampleLight = new osgPPU::UnitInResampleOut();
	{
		_unitResampleLight->setFactorX(0.5);
		_unitResampleLight->setFactorY(0.5);
	}

	_shaderGaussX = new osgPPU::ShaderAttribute();
	_shaderGaussY = new osgPPU::ShaderAttribute();
	{
		_shaderGaussX->addShader(shaderGaussConvolutionVp);
		_shaderGaussX->addShader(shaderGaussConvolution1dxFp);

		_shaderGaussX->add("sigma", osg::Uniform::FLOAT);
		_shaderGaussX->add("radius", osg::Uniform::FLOAT);
		_shaderGaussX->add("texUnit0", osg::Uniform::SAMPLER_2D);

		_shaderGaussX->set("sigma", _gaussSigma);
		_shaderGaussX->set("radius", _gaussRadius);
		_shaderGaussX->set("texUnit0", 0);

		_shaderGaussY->addShader(shaderGaussConvolutionVp);
		_shaderGaussY->addShader(shaderGaussConvolution1dyFp);

		_shaderGaussY->add("sigma", osg::Uniform::FLOAT);
		_shaderGaussY->add("radius", osg::Uniform::FLOAT);
		_shaderGaussY->add("texUnit0", osg::Uniform::SAMPLER_2D);

		_shaderGaussY->set("sigma", _gaussSigma);
		_shaderGaussY->set("radius", _gaussRadius);
		_shaderGaussY->set("texUnit0", 0);
	}

	osgPPU::UnitInOut* blurxlight = new osgPPU::UnitInOut();
	osgPPU::UnitInOut* blurylight = new osgPPU::UnitInOut();
	{
		blurxlight->getOrCreateStateSet()->setAttributeAndModes(_shaderGaussX);
		blurylight->getOrCreateStateSet()->setAttributeAndModes(_shaderGaussY);
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
		blurxstrong->getOrCreateStateSet()->setAttributeAndModes(_shaderGaussX);
		blurystrong->getOrCreateStateSet()->setAttributeAndModes(_shaderGaussY);
	}
	_unitResampleStrong->addChild(blurxstrong);
	blurxstrong->addChild(blurystrong);

	_unitDof = new osgPPU::UnitInOut();
	{
		_shaderDof = new osgPPU::ShaderAttribute();
		_shaderDof->addShader(shaderDepthOfFieldFp);

		_shaderDof->add("focalLength", osg::Uniform::FLOAT);
		_shaderDof->add("focalRange", osg::Uniform::FLOAT);
		_shaderDof->add("zNear", osg::Uniform::FLOAT);
		_shaderDof->add("zFar", osg::Uniform::FLOAT);

		_shaderDof->set("focalLength", _focalLength);
		_shaderDof->set("focalRange", _focalRange);
		_shaderDof->set("zNear", _zNear);
		_shaderDof->set("zFar", _zFar);

		_unitDof->getOrCreateStateSet()->setAttributeAndModes(_shaderDof);
		_unitDof->setInputTextureIndexForViewportReference(0);

		_unitDof->setInputToUniform(blurylight, "texBlurredColorMap", true);
		_unitDof->setInputToUniform(blurystrong, "texStrongBlurredColorMap", true);
	}
}