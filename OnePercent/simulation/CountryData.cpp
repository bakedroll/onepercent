#include "CountryData.h"

#include "core/Globals.h"
#include "SimulationVisitor.h"
#include "SkillBranch.h"

#include <osgGaming/Helper.h>

using namespace osgGaming;
using namespace osg;
using namespace std;

namespace onep
{

  float NeighborCountryInfo::getRelation()
  {
    return m_relation;
  }

  void NeighborCountryInfo::setRelation(float relation)
  {
    m_relation = relation;
  }

  struct CountryData::Impl
  {
    Impl(osg::ref_ptr<osgGaming::PropertiesManager> propertiesManager,
      osg::ref_ptr<SkillBranchContainer> skillBranchContainer,
      string name,
      int id,
      float population,
      float wealth,
      Vec2f centerLatLong,
      Vec2f size)
      : m_name(name)
      , skillBranchContainer(skillBranchContainer)
      , m_values(new CountryValues(propertiesManager, skillBranchContainer, wealth))
      , m_populationInMio(population)
      , m_id(id)
      , m_centerLatLong(centerLatLong)
      , m_size(size)
      , m_earthRadius(propertiesManager->getValue<float>(Param_EarthRadiusName))
      , m_cameraZoom(propertiesManager->getValue<float>(Param_CameraCountryZoomName))
    {}

    std::string m_name;

    SkillBranchContainer::Ptr skillBranchContainer;

    CountryValues::Ptr m_values;
    Neighbor::List m_neighbors;

    float m_populationInMio;

    SkillBranchesActivated m_skillBranches;

    int m_id;
    osg::Vec2f m_centerLatLong;
    osg::Vec2f m_size;

    float m_earthRadius;
    float m_cameraZoom;

    void step()
    {
      ProgressingValue<float>* dept = m_values->getValue<float>(VALUE_DEPT);
      ProgressingValue<float>* anger = m_values->getValue<float>(VALUE_ANGER);
      ProgressingValue<float>* interest = m_values->getValue<float>(VALUE_INTEREST);
      ProgressingValue<float>* buyingPower = m_values->getValue<float>(VALUE_BUYING_POWER);

      SkillBranch::Ptr branch = skillBranchContainer->getBranchByName("banks");
      if (!branch.valid())
      {
        assert(false);
        return;
      }

      if (m_skillBranches.oActivated[branch->getBranchId()]->get())
      {
        dept->prepare(dept->getValue() * interest->getValue() + buyingPower->getValue(), METHOD_SET_BALANCE);
      }

      // rel dept * 0.1
      anger->prepare(dept->getValue() / dept->getMax() * 0.1f, METHOD_SET_BALANCE);

      m_values->getContainer()->step();

      int n = skillBranchContainer->getNumBranches();
      for (int i = 0; i < n; i++)
      {
        if (!m_skillBranches.oActivated[i]->get() && m_values->getBranchValue<float>(VALUE_PROPAGATED, i)->full())
          m_skillBranches.oActivated[i]->set(true);
      }
    }

    void affectNeighbors()
    {
      int n = skillBranchContainer->getNumBranches();
      for (int i = 0; i < n; i++)
      {
        if (m_skillBranches.oActivated[i]->get())
        {
          float propagation = m_values->getBranchValue<float>(VALUE_PROPAGATION, i)->getValue();

          for (Neighbor::List::iterator it = m_neighbors.begin(); it != m_neighbors.end(); ++it)
          {
            if (!it->country->getSkillBranchActivated(i))
              it->country->getValues()->getBranchValue<float>(VALUE_PROPAGATED, i)->prepare(propagation, METHOD_ADD_INFLUENCE);
          }
        }
      }
    }
  };

  CountryData::CountryData(
    osg::ref_ptr<osgGaming::PropertiesManager> propertiesManager,
    osg::ref_ptr<SkillBranchContainer> skillBranchContainer,
    string name,
    int id,
    float population,
    float wealth,
    Vec2f centerLatLong,
    Vec2f size)
    : Group()
    , SimulationCallback()
    , m(new Impl(
      propertiesManager,
      skillBranchContainer,
      name,
      id,
      population,
      wealth,
      centerLatLong,
      size))
  {
    int n = m->skillBranchContainer->getNumBranches();
    for (int i = 0; i < n; i++)
      m->m_skillBranches.oActivated[i] = new osgGaming::InitializedObservable<bool, false>();

    setUpdateCallback(new Callback());
  }

  CountryData::~CountryData()
  {
  }

  void CountryData::addNeighbor(Neighbor neighbor)
  {
    m->m_neighbors.push_back(neighbor);
  }

  void CountryData::setSkillBranchActivated(int type, bool activated)
  {
    m->m_skillBranches.oActivated[type]->set(activated);
  }

  string CountryData::getCountryName()
  {
    return m->m_name;
  }

  int CountryData::getId()
  {
    return m->m_id;
  }

  Vec2f CountryData::getCenterLatLong()
  {
    return m->m_centerLatLong;
  }

  Vec2f CountryData::getSize()
  {
    return m->m_size;
  }

  Vec2f CountryData::getSurfaceSize()
  {
    return Vec2f(
      2.0f * C_PI * sin(C_PI / 2.0f - abs(m->m_centerLatLong.x())) * m->m_earthRadius * m->m_size.x(),
      C_PI * m->m_earthRadius * m->m_size.y());
  }

  float CountryData::getOptimalCameraDistance(float angle, float ratio)
  {
    Vec2f surfaceSize = getSurfaceSize();

    float hdistance = surfaceSize.x() * m->m_cameraZoom / (2.0f * tan(angle * ratio * C_PI / 360.0f)) + m->m_earthRadius;
    float vdistance = surfaceSize.y() * m->m_cameraZoom / (2.0f * tan(angle * C_PI / 360.0f)) + m->m_earthRadius;

    return max(hdistance, vdistance);
  }

  CountryValues::Ptr CountryData::getValues()
  {
    return m->m_values;
  }

  bool CountryData::getSkillBranchActivated(int id)
  {
    return m->m_skillBranches.oActivated[id]->get();
  }

  osgGaming::Observable<bool>::Ptr CountryData::getSkillBranchActivatedObservable(int type)
  {
    return m->m_skillBranches.oActivated[type];
  }

  bool CountryData::anySkillBranchActivated()
  {
    int n = m->skillBranchContainer->getNumBranches();
    for (int i = 0; i < n; i++)
    {
      if (m->m_skillBranches.oActivated[i]->get())
        return true;
    }

    return false;
  }

  bool CountryData::callback(SimulationVisitor* visitor)
  {
    if (visitor->getType() == SimulationVisitor::PROGRESS_COUNTRIES)
    {
      m->step();
      return false; // don't traverse
    }

    if (visitor->getType() == SimulationVisitor::AFFECT_NEIGHBORS)
    {
      m->affectNeighbors();
      return false; // don't traverse
    }

    visitor->setActivatedBranches(&m->m_skillBranches);
    visitor->setCountryValues(m->m_values);

    return true;
  }

}
