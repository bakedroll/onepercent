#pragma once

#include <osgGaming/PostProcessingEffect.h>

#include <osgPPU/ShaderAttribute.h>
#include <osgPPU/UnitInResampleOut.h>

namespace osgGaming
{
	class DepthOfFieldEffect : public PostProcessingEffect
	{
	public:
		DepthOfFieldEffect(float zNear, float zFar);

		virtual InitialUnitList getInitialUnits() override;
		virtual osg::ref_ptr<osgPPU::Unit> getResultUnit() override;
		virtual InputToUniformList getInputToUniform() override;

		void setFocalLengthRange(float length, float range);

	protected:
		virtual void initializeUnits() override;

	private:
		float _gaussSigma;
		float _gaussRadius;
		float _focalLength;
		float _focalRange;
		float _zNear;
		float _zFar;

		osg::ref_ptr<osgPPU::ShaderAttribute> _dofShaderAttribute;
		osg::ref_ptr<osgPPU::UnitInResampleOut> _unitResampleLight;
		osg::ref_ptr<osgPPU::UnitInResampleOut> _unitResampleStrong;
		osg::ref_ptr<osgPPU::UnitInOut> _unitDof;
	};
}