#pragma once

#include "nodes/CountryPresenter.h"

#include <osgGaming/Injector.h>

#include <osg/PositionAttitudeTransform>

namespace onep
{
	class CountryNameOverlay : public osg::PositionAttitudeTransform
	{
	public:
		CountryNameOverlay(osgGaming::Injector& injector);
    ~CountryNameOverlay();

		bool getEnabled();

    void initialize(const CountryPresenter::Map& countryPresenters);
		void setEnabled(bool enabled);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
