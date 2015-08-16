#pragma once

#include "Country.h"

#include <osg/Referenced>

namespace onep
{
	class AbstractSkill : public osg::Referenced
	{
	public:
		void takeEffect(osg::ref_ptr<Country> country);
	};
}