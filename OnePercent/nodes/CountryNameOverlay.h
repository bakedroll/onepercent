#pragma once

#include "simulation/CountryData.h"
#include "nodes/CountryMesh.h"

#include <osg/PositionAttitudeTransform>
#include <osg/Switch>

namespace onep
{
	class CountryNameOverlay : public osg::PositionAttitudeTransform
	{
	public:
		CountryNameOverlay();

		bool getEnabled();

    void setCountryMap(const CountryMesh::Map& countryMap);
		void setEnabled(bool enabled);

	private:
		osg::ref_ptr<osg::Switch> _switch;
    const CountryMesh::Map* _countryMap;

		bool _enabled;
	};
}