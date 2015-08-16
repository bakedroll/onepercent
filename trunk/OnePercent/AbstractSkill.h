#pragma once

#include "Country.h"

#include <osg/Referenced>

namespace onep
{
	typedef enum _skillType
	{
		INTEREST_I,
		INTEREST_II
	} SkillType;

	class AbstractSkill : public osg::Referenced
	{
	public:
		void takeEffect(osg::ref_ptr<Country> country);
	};
}