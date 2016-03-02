#pragma once

//#include "Country.h"

#include <osg/Referenced>
#include <osg/ref_ptr>

#include <vector>

namespace onep
{
  class Country;

	class Skill : public osg::Referenced
	{
	public:
    typedef osg::ref_ptr<Skill> Ptr;
    typedef std::vector<Ptr> List;

		Skill(std::string name);

		void setAnger(float anger);
		void setInterest(float interest);
    void setPropagation(float propagation);
		void setActivated(bool activated);

		std::string getName();
		float getAnger();
		float getInterest();
    float getPropagation();
		bool getActivated();

		void takeEffect(osg::ref_ptr<Country> country);
	  
	  std::string _name;
		float _anger;
    float m_propagation;
		float _interest;

		bool _activated;
	};
}