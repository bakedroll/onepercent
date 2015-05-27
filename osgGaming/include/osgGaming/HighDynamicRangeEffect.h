#pragma once

#include <osgGaming/SimulationCallback.h>
#include <osgGaming/PostProcessingEffect.h>
#include <osgPPU/UnitInResampleOut.h>

namespace osgGaming
{
	class HighDynamicRangeEffect : public PostProcessingEffect, public SimulationCallback
	{
	public:
		HighDynamicRangeEffect();

		virtual InitialUnitList getInitialUnits() override;
		virtual osg::ref_ptr<osgPPU::Unit> getResultUnit() override;
		virtual InputToUniformList getInputToUniform() override;

		virtual void action(osg::Node* node, osg::NodeVisitor* nv, double simTime, double timeDiff) override;

	protected:
		virtual void initializeUnits() override;

	private:
		float _midGrey;
		float _hdrBlurSigma;
		float _hdrBlurRadius;
		float _glareFactor;
		float _adaptFactor;
		float _minLuminance;
		float _maxLuminance;

		osg::ref_ptr<osgPPU::UnitInResampleOut> _unitResample;
		osg::ref_ptr<osgPPU::UnitInOut> _unitHdr;
		osg::ref_ptr<osgPPU::UnitInOut> _unitAdaptedLuminance;
	};
}