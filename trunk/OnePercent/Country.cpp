#include "Country.h"

using namespace onep;
using namespace osg;
using namespace std;

Country::Country(string name, unsigned char id, float population, int bip)
	: Referenced(),
	  _name(name),
	  _id(id),
	  _populationInMio(population),
	  _bip(bip),
	  _anger(0.0f),
	  _buyingPower(0.0f),
	  _dept(0)
{
	for (int i = 0; i < SkillBranchCount; i++)
	{
		_skillBranchActivated[i] = false;
	}
}

void Country::setSkillBranchActivated(SkillBranchType type, bool activated)
{
	_skillBranchActivated[type] = activated;
}

string Country::getCountryName()
{
	return _name;
}

unsigned char Country::getId()
{
	return _id;
}

int Country::getBip()
{
	return _bip;
}

bool Country::getSKillBranchActivated(SkillBranchType type)
{
	return _skillBranchActivated[type];
}