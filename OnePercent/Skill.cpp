#include "Skill.h"

#include "Country.h"

using namespace onep;
using namespace std;
using namespace osg;

Skill::Skill(string name)
	: Referenced(),
	  _name(name),
	  _anger(0.0f),
	  _interest(0.0f),
	  _activated(false)
{
}

void Skill::setAnger(float anger)
{
	_anger = anger;
}

void Skill::setInterest(float interest)
{
	_interest = interest;
}

void Skill::setPropagation(float propagation)
{
  m_propagation = propagation;
}

void Skill::setActivated(bool activated)
{
	_activated = activated;
}

string Skill::getName()
{
	return _name;
}

float Skill::getAnger()
{
	return _anger;
}

float Skill::getInterest()
{
	return _interest;
}

float Skill::getPropagation()
{
  return m_propagation;
}

bool Skill::getActivated()
{
	return _activated;
}

void Skill::takeEffect(ref_ptr<Country> country)
{
	country->addAngerInfluence(_anger);
	country->addInterestChange(_interest);
  //country->getAffectNeighborValue()
}