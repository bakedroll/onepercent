#pragma once

#include <string>

#include <osgGA/GUIEventHandler>

#include <osgGaming/PostProcessingEffect.h>

#include <osgPPU/UnitInOut.h>
#include <osgPPU/ShaderAttribute.h>

namespace osgGaming
{
	class FastApproximateAntiAliasingEffectCallback : public osgGA::GUIEventHandler
	{
	public:
		FastApproximateAntiAliasingEffectCallback(osg::ref_ptr<osgPPU::ShaderAttribute> shaderFxaa, osg::Vec2f resolution);

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

	private:
		osg::ref_ptr<osgPPU::ShaderAttribute> _shaderFxaa;
		osg::Vec2f _resolution;
	};

	class FastApproximateAntiAliasingEffect : public PostProcessingEffect
	{
	public:
		static const std::string NAME;

		FastApproximateAntiAliasingEffect(osg::Vec2f resolution);

		virtual std::string getName() override;
		virtual InitialUnitList getInitialUnits() override;
		virtual osg::ref_ptr<osgPPU::Unit> getResultUnit() override;
		virtual InputToUniformList getInputToUniform() override;

		void setResolution(osg::Vec2f resolution);

	protected:
		virtual void initializeUnits() override;

	private:
		osg::ref_ptr<osgPPU::UnitInOut> _unitFxaa;
		osg::Vec2f _resolution;
		osg::ref_ptr<osgPPU::ShaderAttribute> _shaderFxaa;
	};
}