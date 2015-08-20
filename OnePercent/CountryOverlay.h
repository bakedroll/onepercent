#pragma once

#include "Country.h"

#include <osg/PositionAttitudeTransform>
#include <osg/Switch>

namespace onep
{
	class CountryOverlay : public osg::PositionAttitudeTransform
	{
	public:
		CountryOverlay();

		bool getEnabled();

		void setCountryMap(const Country::Map& countryMap);
		void setEnabled(bool enabled);

	private:
		osg::ref_ptr<osg::Switch> _switch;
		const Country::Map* _countryMap;

		bool _enabled;
	};
}