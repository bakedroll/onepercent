#include "Country.h"
#include "Globals.h"

#include <osgGaming/Property.h>
#include <osgGaming/Helper.h>

using namespace onep;
using namespace osgGaming;
using namespace osg;
using namespace std;

float NeighborCountryInfo::getRelation()
{
	return _relation;
}

void NeighborCountryInfo::setRelation(float relation)
{
	_relation = relation;
}

Country::Country(string name, unsigned char id, float population, float wealth, Vec2f centerLatLong, Vec2f size)
	: Referenced()
  , m_name(name)
  , m_valueAnger(new ProgressingValue<float>(0.0f, 1.0f, 0.0f))
  , m_valueDept(new ProgressingValue<float>(0.0f, wealth, 0.0f))
  , m_valueInterest(new ProgressingValue<float>(0.0f, 1.0f, 0.0f))
  , m_valueBuyingPower(new ProgressingValue<float>(0.0f, 1000.0f, 0.0f))
  , m_populationInMio(population)
  , m_id(id)
  , m_centerLatLong(centerLatLong)
  , m_size(size)
{
	for (int i = 0; i < SkillBranchCount; i++)
	{
		m_skillBranchActivated[i] = false;
	}

  m_valueContainer.registerValue(m_valueAnger);
  m_valueContainer.registerValue(m_valueDept);
  m_valueContainer.registerValue(m_valueInterest);
  m_valueContainer.registerValue(m_valueBuyingPower);

  m_valueInterest->setValue(0.05f);
  m_valueBuyingPower->setValue(~Property<float, Param_MechanicsStartBuyingPowerName>());
}

void Country::addAngerInfluence(float influence)
{
  m_valueAnger->addInfluence(influence);
}

void Country::addInterestChange(float change)
{
  m_valueInterest->addChange(change);
}

void Country::setSkillBranchActivated(SkillBranchType type, bool activated)
{
	m_skillBranchActivated[type] = activated;
}

string Country::getCountryName()
{
	return m_name;
}

unsigned char Country::getId()
{
	return m_id;
}

Vec2f Country::getCenterLatLong()
{
	return m_centerLatLong;
}

Vec2f Country::getSize()
{
	return m_size;
}

Vec2f Country::getSurfaceSize()
{
	float earthRadius = ~Property<float, Param_EarthRadiusName>();

	return Vec2f(
		2.0f * C_PI * sin(C_PI / 2.0f - abs(m_centerLatLong.x())) * earthRadius * m_size.x(),
		C_PI * earthRadius * m_size.y());
}

float Country::getOptimalCameraDistance(float angle, float ratio)
{
	float cameraZoom = ~Property<float, Param_CameraCountryZoomName>();
	float earthRadius = ~Property<float, Param_EarthRadiusName>();

	Vec2f surfaceSize = getSurfaceSize();

	float hdistance = surfaceSize.x() * cameraZoom / (2.0f * tan(angle * ratio * C_PI / 360.0f)) + earthRadius;
	float vdistance = surfaceSize.y() * cameraZoom / (2.0f * tan(angle * C_PI / 360.0f)) + earthRadius;

	return max(hdistance, vdistance);
}

float Country::getWealth()
{
	return m_valueDept->getMax();
}

float Country::getDept()
{
	return m_valueDept->getValue();
}

float Country::getDeptBalance()
{
	return m_valueDept->getBalance();
}

float Country::getRelativeDept()
{
	return m_valueDept->getValue() / m_valueDept->getMax();
}

float Country::getAnger()
{
	return m_valueAnger->getValue();
}

float Country::getAngerBalance()
{
	return m_valueAnger->getBalance();
}

bool Country::getSKillBranchActivated(SkillBranchType type)
{
	return m_skillBranchActivated[type];
}

bool Country::anySkillBranchActivated()
{
	for (int i = 0; i < SkillBranchCount; i++)
	{
		if (m_skillBranchActivated[i])
		{
			return true;
		}
	}

	return false;
}

void Country::step()
{
	if (m_skillBranchActivated[BRANCH_BANKS])
	{
    m_valueDept->setBalance(m_valueDept->getValue() * m_valueInterest->getValue() + m_valueBuyingPower->getValue());
	}

	m_valueAnger->setBalance(getRelativeDept() * 0.1f);

  m_valueContainer.step();
}