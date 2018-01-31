#pragma once

#include <memory>

#include <osgGA/GUIEventHandler>

#include <osgGaming/PostProcessingEffect.h>

#include <osgPPU/UnitInOut.h>
#include <osgPPU/ShaderAttribute.h>

namespace osgGaming
{
  class Injector;

	class FastApproximateAntiAliasingEffect : public PostProcessingEffect
	{
	public:
		static const std::string NAME;

		FastApproximateAntiAliasingEffect(Injector& injector);
    ~FastApproximateAntiAliasingEffect();

		virtual std::string getName() override;
		virtual InitialUnitList getInitialUnits() override;
		virtual osg::ref_ptr<osgPPU::Unit> getResultUnit() override;
		virtual InputToUniformList getInputToUniform() override;

		void setResolution(osg::Vec2f resolution);

	protected:
		virtual void initializeUnits() override;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}