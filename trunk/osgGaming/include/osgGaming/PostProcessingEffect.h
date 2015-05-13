#pragma once

#include <vector>

#include <osg/Referenced>
#include <osg/Camera>
#include <osgPPU/Unit.h>

namespace osgGaming
{
	class PostProcessingEffect : public osg::Referenced
	{
	public:
		typedef struct _initialUnit
		{
			osg::ref_ptr<osgPPU::Unit> unit;
			osg::Camera::BufferComponent component;
		} InitialUnit;

		typedef std::vector<InitialUnit> InitialUnitList;

		PostProcessingEffect();

		void initialize();

		virtual InitialUnitList getInitialUnits();
		virtual osg::ref_ptr<osgPPU::Unit> getOngoingUnit() = 0;
		virtual osg::ref_ptr<osgPPU::Unit> getResultUnit() = 0;

	protected:
		virtual void initializeUnits() = 0;

	private:
		bool _initialized;
	};
}