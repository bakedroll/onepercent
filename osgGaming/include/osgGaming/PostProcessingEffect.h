#pragma once

#include <vector>
#include <string>

#include <osg/Referenced>
#include <osg/Camera>
#include <osg/Switch>
#include <osgPPU/Unit.h>

namespace osgGaming
{
	class PostProcessingEffect : public osg::Referenced
	{
	public:
		enum class UnitType
		{
			BYPASS_COLOR,
			BYPASS_DEPTH,
			ONGOING_COLOR
		};

		struct InitialUnit
		{
      osg::ref_ptr<osgPPU::Unit> unit;
      UnitType                   type;
    };

		struct InputToUniform
		{
      osg::ref_ptr<osgPPU::Unit> unit;
      std::string                name;
      UnitType                   type;
    };

    using InitialUnitList    = std::vector<InitialUnit>;
    using InputToUniformList = std::vector<InputToUniform>;

    PostProcessingEffect();

		void initialize();
		bool isInitialized();

    virtual std::string                getName()         = 0;
    virtual InitialUnitList            getInitialUnits() = 0;
    virtual osg::ref_ptr<osgPPU::Unit> getResultUnit()   = 0;
    virtual InputToUniformList         getInputToUniform();

  protected:
		virtual void initializeUnits() = 0;

	private:
		bool _initialized;
	};
}