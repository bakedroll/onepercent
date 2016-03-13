#pragma once

#include "simulation/Country.h"

#include <osg/PositionAttitudeTransform>
#include <osg/Switch>
#include "CountryMesh.h"

namespace onep
{
	class CountryOverlay : public osg::PositionAttitudeTransform
	{
	public:
		CountryOverlay();

		bool getEnabled();

    void setCountryMap(const CountryMesh::Map& countryMap);
		void setEnabled(bool enabled);

	private:
		osg::ref_ptr<osg::Switch> _switch;
    const CountryMesh::Map* _countryMap;

		bool _enabled;
	};
}