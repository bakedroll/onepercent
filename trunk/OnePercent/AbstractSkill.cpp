#include "AbstractSkill.h"

using namespace onep;
using namespace std;
using namespace osg;

AbstractSkill::AbstractSkill(string name, Country::SkillBranchType branch)
	: Referenced(),
	  _name(name),
	  _branch(branch),
	  _anger(0.0f),
	  _interest(0.0f),
	  _activated(false)
{
}

void AbstractSkill::setAnger(float anger)
{
	_anger = anger;
}

void AbstractSkill::setInterest(float interest)
{
	_interest = interest;
}

void AbstractSkill::setActivated(bool activated)
{
	_activated = activated;
}

string AbstractSkill::getName()
{
	return _name;
}

Country::SkillBranchType AbstractSkill::getBranch()
{
	return _branch;
}

float AbstractSkill::getAnger()
{
	return _anger;
}

float AbstractSkill::getInterest()
{
	return _interest;
}

bool AbstractSkill::getActivated()
{
	return _activated;
}

void AbstractSkill::takeEffect(ref_ptr<Country> country)
{
	country->addAngerInfluence(_anger);
	country->addInterestChange(_interest);
}