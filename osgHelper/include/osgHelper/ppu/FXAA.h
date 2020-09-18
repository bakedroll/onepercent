#pragma once

#include <osgHelper/ppu/Effect.h>
#include <osgHelper/ioc/Injector.h>

#include <memory>

#include <osgGA/GUIEventHandler>

#include <osgPPU/UnitInOut.h>

namespace osgHelper
{
namespace ppu
{
	class FXAA : public Effect
	{
	public:
		static const std::string Name;

    explicit FXAA(osgHelper::ioc::Injector& injector);
    ~FXAA();

		std::string getName() const override;
		InitialUnitList getInitialUnits() const override;
		osg::ref_ptr<osgPPU::Unit> getResultUnit() const override;
		InputToUniformList getInputToUniform() const override;

		void setResolution(osg::Vec2f resolution);

	protected:
		void initializeUnits() override;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
}