#include "CountryData.h"

#include "core/Globals.h"
#include "SimulationVisitor.h"

#include <osgGaming/Property.h>
#include <osgGaming/Helper.h>

using namespace osgGaming;
using namespace osg;
using namespace std;

namespace onep
{

  float NeighborCountryInfo::getRelation()
  {
    return _relation;
  }

  void NeighborCountryInfo::setRelation(float relation)
  {
    _relation = relation;
  }

  CountryData::CountryData(string name, unsigned char id, float population, float wealth, Vec2f centerLatLong, Vec2f size)
    : Group()
    , SimulationCallback()
    , m_name(name)
    , m_values(new CountryValues(wealth))
    , m_populationInMio(population)
    , m_id(id)
    , m_centerLatLong(centerLatLong)
    , m_size(size)
  {
    for (int i = 0; i < NUM_SKILLBRANCHES; i++)
      m_skillBranches.oActivated[i] = new osgGaming::Observable<bool>(false);

    setUpdateCallback(new Callback());
  }

  void CountryData::addNeighbor(Neighbor neighbor)
  {
    m_neighbors.push_back(neighbor);
  }

  void CountryData::setSkillBranchActivated(int type, bool activated)
  {
    m_skillBranches.oActivated[type]->set(activated);
  }

  string CountryData::getCountryName()
  {
    return m_name;
  }

  unsigned char CountryData::getId()
  {
    return m_id;
  }

  Vec2f CountryData::getCenterLatLong()
  {
    return m_centerLatLong;
  }

  Vec2f CountryData::getSize()
  {
    return m_size;
  }

  Vec2f CountryData::getSurfaceSize()
  {
    float earthRadius = ~Property<float, Param_EarthRadiusName>();

    return Vec2f(
      2.0f * C_PI * sin(C_PI / 2.0f - abs(m_centerLatLong.x())) * earthRadius * m_size.x(),
      C_PI * earthRadius * m_size.y());
  }

  float CountryData::getOptimalCameraDistance(float angle, float ratio)
  {
    float cameraZoom = ~Property<float, Param_CameraCountryZoomName>();
    float earthRadius = ~Property<float, Param_EarthRadiusName>();

    Vec2f surfaceSize = getSurfaceSize();

    float hdistance = surfaceSize.x() * cameraZoom / (2.0f * tan(angle * ratio * C_PI / 360.0f)) + earthRadius;
    float vdistance = surfaceSize.y() * cameraZoom / (2.0f * tan(angle * C_PI / 360.0f)) + earthRadius;

    return max(hdistance, vdistance);
  }

  CountryValues::Ptr CountryData::getValues()
  {
    return m_values;
  }

  bool CountryData::getSkillBranchActivated(int type)
  {
    return m_skillBranches.oActivated[type]->get();
  }

  osgGaming::Observable<bool>::Ptr CountryData::getSkillBranchActivatedObservable(int type)
  {
    return m_skillBranches.oActivated[type];
  }

  bool CountryData::anySkillBranchActivated()
  {
    for (int i = 0; i < NUM_SKILLBRANCHES; i++)
    {
      if (m_skillBranches.oActivated[i]->get())
      {
        return true;
      }
    }

    return false;
  }

  bool CountryData::callback(SimulationVisitor* visitor)
  {
    if (visitor->getType() == SimulationVisitor::PROGRESS_COUNTRIES)
    {
      step();
      return false; // don't traverse
    }

    if (visitor->getType() == SimulationVisitor::AFFECT_NEIGHBORS)
    {
      affectNeighbors();
      return false; // don't traverse
    }

    visitor->setActivatedBranches(&m_skillBranches);
    visitor->setCountryValues(m_values);

    return true;
  }

  void CountryData::step()
  {
    ProgressingValue<float>* dept = m_values->getValue<float>(VALUE_DEPT);
    ProgressingValue<float>* anger = m_values->getValue<float>(VALUE_ANGER);
    ProgressingValue<float>* interest = m_values->getValue<float>(VALUE_INTEREST);
    ProgressingValue<float>* buyingPower = m_values->getValue<float>(VALUE_BUYING_POWER);

    if (m_skillBranches.oActivated[BRANCH_BANKS]->get())
    {
      dept->prepare(dept->getValue() * interest->getValue() + buyingPower->getValue(), METHOD_SET_BALANCE);
    }

    // rel dept * 0.1
    anger->prepare(dept->getValue() / dept->getMax() * 0.1f, METHOD_SET_BALANCE);

    m_values->getContainer()->step();

    for (int i = 0; i < NUM_SKILLBRANCHES; i++)
    {
      if (!m_skillBranches.oActivated[i]->get() && m_values->getBranchValue<float>(VALUE_PROPAGATED, BranchType(i))->full())
        m_skillBranches.oActivated[i]->set(true);
    }
  }

  void CountryData::affectNeighbors()
  {
    for (int i = 0; i < NUM_SKILLBRANCHES; i++)
    {
      if (m_skillBranches.oActivated[i]->get())
      {
        float propagation = m_values->getBranchValue<float>(VALUE_PROPAGATION, BranchType(i))->getValue();

        for (Neighbor::List::iterator it = m_neighbors.begin(); it != m_neighbors.end(); ++it)
        {
          if (!it->country->getSkillBranchActivated(i))
            it->country->getValues()->getBranchValue<float>(VALUE_PROPAGATED, BranchType(i))->prepare(propagation, METHOD_ADD_INFLUENCE);
        }
      }
    }
  }

}