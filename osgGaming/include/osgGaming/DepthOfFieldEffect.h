#pragma once

#include <osgGaming/PostProcessingEffect.h>

#include <osgPPU/ShaderAttribute.h>
#include <osgPPU/UnitInResampleOut.h>

namespace osgGaming
{
	class DepthOfFieldEffect : public PostProcessingEffect
	{
	public:
		static const std::string NAME;

		DepthOfFieldEffect(float zNear, float zFar);

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
		float _gaussSigma;
		float _gaussRadius;
		float _focalLength;
		float _focalRange;
		float _zNear;
		float _zFar;

		osg::ref_ptr<osgPPU::ShaderAttribute> _shaderDof;
		osg::ref_ptr<osgPPU::ShaderAttribute> _shaderGaussX;
		osg::ref_ptr<osgPPU::ShaderAttribute> _shaderGaussY;
		osg::ref_ptr<osgPPU::UnitInResampleOut> _unitResampleLight;
		osg::ref_ptr<osgPPU::UnitInResampleOut> _unitResampleStrong;
		osg::ref_ptr<osgPPU::UnitInOut> _unitDof;
	};
}