#include "Country.h"
#include "Globals.h"
#include "GlobeModel.h"

#include <osgGaming/Parameter.h>
#include <osgGaming/Helper.h>

using namespace onep;
using namespace osgGaming;
using namespace osg;
using namespace std;

Country::Country(string name, unsigned char id, float population, float wealth, Vec2f centerLatLong, Vec2f size)
	: Referenced(),
	  _name(name),
	  _id(id),
	  _centerLatLong(centerLatLong),
	  _size(size),
	  _populationInMio(population),
	  _wealth(wealth),
	  _anger(0.0f),
	  _angerBalance(0.0f),
	  _buyingPower(0.0f),
	  _dept(0.0f),
	  _deptBalance(0.0f),
	  _interest(0.0f)
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

Vec2f Country::getCenterLatLong()
{
	return _centerLatLong;
}

Vec2f Country::getSize()
{
	return _size;
}

Vec2f Country::getSurfaceSize()
{
	float earthRadius = ~Parameter<float, Param_EarthRadiusName>();

	return Vec2f(
		2.0f * C_PI * sin(C_PI / 2.0f - abs(_centerLatLong.x())) * earthRadius * _size.x(),
		C_PI * earthRadius * _size.y());
}

float Country::getOptimalCameraDistance(float angle, float ratio)
{
	float earthRadius = ~Parameter<float, Param_EarthRadiusName>();

	Vec2f surfaceSize = getSurfaceSize();

	float hdistance = surfaceSize.x() * 1.5f / (2.0f * tan(angle * ratio * C_PI / 360.0f)) + earthRadius;
	float vdistance = surfaceSize.y() * 1.5f / (2.0f * tan(angle * C_PI / 360.0f)) + earthRadius;

	return max(hdistance, vdistance);
}

float Country::getWealth()
{
	return _wealth;
}

float Country::getDept()
{
	return _dept;
}

float Country::getDeptBalance()
{
	return _deptBalance;
}

float Country::getRelativeDept()
{
	return _dept / _wealth;
}

float Country::getAnger()
{
	return _anger;
}

float Country::getAngerBalance()
{
	return _angerBalance;
}

bool Country::getSKillBranchActivated(SkillBranchType type)
{
	return _skillBranchActivated[type];
}

bool Country::anySkillBranchActivated()
{
	for (int i = 0; i < SkillBranchCount; i++)
	{
		if (_skillBranchActivated[i])
		{
			return true;
		}
	}

	return false;
}

void Country::step()
{
	if (_skillBranchActivated[BRANCH_BANKS])
	{
		_buyingPower = ~Parameter<float, Param_MechanicsStartBuyingPowerName>();
		_interest = 0.05f;

		_deptBalance = _dept * _interest + _buyingPower;
		_dept += _deptBalance;

		_dept = clampBetween(_dept, 0.0f, _wealth);
	}

	_angerBalance = getRelativeDept() * 0.1f;
	_anger += _angerBalance;

	_anger = clampBetween(_anger, 0.0f, 1.0f);
}