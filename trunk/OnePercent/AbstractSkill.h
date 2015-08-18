#pragma once

#include "Country.h"

#include <osg/Referenced>

namespace onep
{
	class AbstractSkill : public osg::Referenced
	{
	public:
		typedef std::map<int, osg::ref_ptr<AbstractSkill>> Map;

		AbstractSkill(std::string name, Country::SkillBranchType branch);

		void setAnger(float anger);
		void setInterest(float interest);
		void setActivated(bool activated);

		std::string getName();
		Country::SkillBranchType getBranch();
		float getAnger();
		float getInterest();
		bool getActivated();

		void takeEffect(osg::ref_ptr<Country> country);

		std::string _name;
		Country::SkillBranchType _branch;
		float _anger;
		float _interest;

		bool _activated;
	};
}