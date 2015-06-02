#pragma once

#include <osgGaming/SimulationCallback.h>
#include <osgGaming/PostProcessingEffect.h>

#include <osgPPU/UnitInResampleOut.h>
#include <osgPPU/ShaderAttribute.h>

namespace osgGaming
{
	class HighDynamicRangeEffect : public PostProcessingEffect, public SimulationCallback
	{
	public:
		static const std::string NAME;

		HighDynamicRangeEffect();

		virtual std::string getName() override;
		virtual InitialUnitList getInitialUnits() override;
		virtual osg::ref_ptr<osgPPU::Unit> getResultUnit() override;
		virtual InputToUniformList getInputToUniform() override;

		virtual void action(osg::Node* node, osg::NodeVisitor* nv, double simTime, double timeDiff) override;

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

		osg::ref_ptr<osgPPU::ShaderAttribute> _shaderBrightpass;
		osg::ref_ptr<osgPPU::ShaderAttribute> _shaderHdr;
		osg::ref_ptr<osgPPU::ShaderAttribute> _shaderGaussX;
		osg::ref_ptr<osgPPU::ShaderAttribute> _shaderGaussY;
		osg::ref_ptr<osgPPU::ShaderAttribute> _shaderAdapted;
	};
}