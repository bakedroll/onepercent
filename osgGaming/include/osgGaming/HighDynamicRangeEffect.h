#pragma once

#include <memory>

#include <osgGaming/SimulationCallback.h>
#include <osgGaming/PostProcessingEffect.h>

namespace osgGaming
{
  class Injector;

  class HighDynamicRangeEffect : public PostProcessingEffect
	{
	public:
		static const std::string NAME;

		HighDynamicRangeEffect(Injector& injector);
    ~HighDynamicRangeEffect();

		virtual std::string getName() override;
		virtual InitialUnitList getInitialUnits() override;
		virtual osg::ref_ptr<osgPPU::Unit> getResultUnit() override;
		virtual InputToUniformList getInputToUniform() override;

		void setMidGrey(float midGrey);
		void setBlurSigma(float blurSigma);
		void setBlurRadius(float blurRadius);
		void setGlareFactor(float glareFactor);
		void setAdaptFactor(float adaptFactor);
		void setMinLuminance(float minLuminance);
		void setMaxLuminance(float maxLuminance);

		float getMidGrey();
		float getBlurSigma();
		float getBlurRadius();
		float getGlareFactor();
		float getAdaptFactor();
		float getMinLuminance();
		float getMaxLuminance();

	protected:
		virtual void initializeUnits() override;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}