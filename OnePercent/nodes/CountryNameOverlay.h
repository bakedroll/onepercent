#pragma once

#include <osgGaming/Injector.h>

#include "nodes/CountryMesh.h"

#include <osg/PositionAttitudeTransform>
#include <osg/Switch>

namespace onep
{
	class CountryNameOverlay : public osg::PositionAttitudeTransform
	{
	public:
		CountryNameOverlay(osgGaming::Injector& injector);
    ~CountryNameOverlay();

		bool getEnabled();

    void setCountryMap(const CountryMesh::Map& countryMap);
		void setEnabled(bool enabled);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}