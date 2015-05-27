#pragma once

#include <vector>

#include <osg/Referenced>
#include <osg/Camera>
#include <osg/Switch>
#include <osgPPU/Unit.h>

namespace osgGaming
{
	class PostProcessingEffect : public osg::Referenced
	{
	public:
		typedef enum _unitType
		{
			BYPASS_COLOR,
			BYPASS_DEPTH,
			ONGOING_COLOR
		} UnitType;

		typedef struct _initialUnit
		{
			osg::ref_ptr<osgPPU::Unit> unit;
			UnitType type;
		} InitialUnit;

		typedef struct _inputToUniform
		{
			osg::ref_ptr<osgPPU::Unit> unit;
			std::string name;
			UnitType type;
		} InputToUniform;

		typedef std::vector<InitialUnit> InitialUnitList;
		typedef std::vector<InputToUniform> InputToUniformList;
		typedef std::vector<osg::ref_ptr<osg::Switch>> SwitchList;

		PostProcessingEffect();

		void initialize();

		virtual InitialUnitList getInitialUnits() = 0;
		virtual osg::ref_ptr<osgPPU::Unit> getResultUnit() = 0;
		virtual InputToUniformList getInputToUniform();

	protected:
		virtual void initializeUnits() = 0;

	private:
		bool _initialized;
	};
}