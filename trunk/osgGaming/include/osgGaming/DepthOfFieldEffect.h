#pragma once

#include <memory>

#include <osgGaming/PostProcessingEffect.h>

#include <osgPPU/ShaderAttribute.h>
#include <osgPPU/UnitInResampleOut.h>

namespace osgGaming
{
  class Injector;

  class DepthOfFieldEffect : public PostProcessingEffect
	{
	public:
		static const std::string NAME;

		DepthOfFieldEffect(Injector& injector);
    ~DepthOfFieldEffect();

		virtual std::string getName() override;
		virtual InitialUnitList getInitialUnits() override;
		virtual osg::ref_ptr<osgPPU::Unit> getResultUnit() override;
		virtual InputToUniformList getInputToUniform() override;

		void setGaussSigma(float gaussSigma);
		void setGaussRadius(float gaussRadius);
		void setFocalLength(float focalLength);
		void setFocalRange(float focalRange);
		void setZNear(float zNear);
		void setZFar(float zFar);

		float getGaussSigma();
		float getGaussRadius();
		float getFocalLength();
		float getFocalRange();
		float getZNear();
		float getZFar();

	protected:
		virtual void initializeUnits() override;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}