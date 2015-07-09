#include <osgGaming/FastApproximateAntiAliasingEffect.h>
#include <osgGaming/ShaderFactory.h>
#include <osgGaming/StaticResources.h>

using namespace osgGaming;
using namespace osg;
using namespace std;

FastApproximateAntiAliasingEffectCallback::FastApproximateAntiAliasingEffectCallback(ref_ptr<osgPPU::ShaderAttribute> shaderFxaa, Vec2f resolution)
	: osgGA::GUIEventHandler(),
	  _shaderFxaa(shaderFxaa),
	  _resolution(resolution)
{

}

bool FastApproximateAntiAliasingEffectCallback::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::RESIZE:

		int width = ea.getWindowWidth();
		int height = ea.getWindowHeight();

		if (width != (int)_resolution.x() || height != (int)_resolution.y())
		{
			_resolution = Vec2f((float)width, (float)height);

			_shaderFxaa->set("rt_w", _resolution.x());
			_shaderFxaa->set("rt_h", _resolution.y());

			return true;
		}
	}

	return false;
}

const string FastApproximateAntiAliasingEffect::NAME = "fxaaEffect";

FastApproximateAntiAliasingEffect::FastApproximateAntiAliasingEffect(Vec2f resolution)
	: PostProcessingEffect(),
	  _resolution(resolution)
{

}

string FastApproximateAntiAliasingEffect::getName()
{
	return NAME;
}

FastApproximateAntiAliasingEffect::InitialUnitList FastApproximateAntiAliasingEffect::getInitialUnits()
{
	PostProcessingEffect::InitialUnitList list;

	return list;
}

ref_ptr<osgPPU::Unit> FastApproximateAntiAliasingEffect::getResultUnit()
{
	return _unitFxaa;
}

FastApproximateAntiAliasingEffect::InputToUniformList FastApproximateAntiAliasingEffect::getInputToUniform()
{
	PostProcessingEffect::InputToUniformList list;

	InputToUniform ituBypass;
	ituBypass.name = "tex0";
	ituBypass.type = PostProcessingEffect::ONGOING_COLOR;
	ituBypass.unit = _unitFxaa;

	list.push_back(ituBypass);

	return list;
}

void FastApproximateAntiAliasingEffect::setResolution(Vec2f resolution)
{
	_resolution = resolution;

	_shaderFxaa->set("rt_w", _resolution.x());
	_shaderFxaa->set("rt_h", _resolution.y());
}

void FastApproximateAntiAliasingEffect::initializeUnits()
{
	ref_ptr<Shader> shaderFxaaFp = ShaderFactory::getInstance()->fromSourceText("ShaderFxaaFp", StaticResources::ShaderFxaaFp, Shader::FRAGMENT);
	ref_ptr<Shader> shaderFxaaVp = ShaderFactory::getInstance()->fromSourceText("ShaderFxaaVp", StaticResources::ShaderFxaaVp, Shader::VERTEX);

	_unitFxaa = new osgPPU::UnitInOut();
	{
		_shaderFxaa = new osgPPU::ShaderAttribute();
		_shaderFxaa->addShader(shaderFxaaFp);
		_shaderFxaa->addShader(shaderFxaaVp);

		_shaderFxaa->add("rt_w", osg::Uniform::FLOAT);
		_shaderFxaa->add("rt_h", osg::Uniform::FLOAT);

		_shaderFxaa->set("rt_w", _resolution.x());
		_shaderFxaa->set("rt_h", _resolution.y());

		_unitFxaa->getOrCreateStateSet()->setAttributeAndModes(_shaderFxaa);

		_unitFxaa->setEventCallback(new FastApproximateAntiAliasingEffectCallback(_shaderFxaa, _resolution));
	}
}