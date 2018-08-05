#pragma once

#include <osgGaming/Injector.h>

#include "nodes/CountryNode.h"

#include <osg/PositionAttitudeTransform>

namespace onep
{
	class CountryNameOverlay : public osg::PositionAttitudeTransform
	{
	public:
		CountryNameOverlay(osgGaming::Injector& injector);
    ~CountryNameOverlay();

		bool getEnabled();

    void setCountryMap(const CountryNode::Map& countryMap);
		void setEnabled(bool enabled);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}