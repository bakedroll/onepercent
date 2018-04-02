#include "CountryData.h"

#include "core/Globals.h"
#include "SimulationVisitor.h"

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
      osg::ref_ptr<SkillsContainer> skillsContainer,
      string name,
      int id,
      float population,
      float wealth,
      Vec2f centerLatLong,
      Vec2f size)
      : name(name)
      , skillsContainer(skillsContainer)
      , values(new CountryValues(propertiesManager, skillsContainer, wealth))
      , populationInMio(population)
      , id(id)
      , centerLatLong(centerLatLong)
      , size(size)
      , earthRadius(propertiesManager->getValue<float>(Param_EarthRadiusName))
      , cameraZoom(propertiesManager->getValue<float>(Param_CameraCountryZoomName))
    {}

    std::string name;

    SkillsContainer::Ptr skillsContainer;

    CountryValues::Ptr values;
    Neighbor::List m_neighbors;

    float populationInMio;

    int id;
    osg::Vec2f centerLatLong;
    osg::Vec2f size;

    float earthRadius;
    float cameraZoom;

    void step()
    {/*
      ProgressingValue<float>* dept = values->getValue<float>(VALUE_DEPT);
      ProgressingValue<float>* anger = values->getValue<float>(VALUE_ANGER);
      ProgressingValue<float>* interest = values->getValue<float>(VALUE_INTEREST);
      ProgressingValue<float>* buyingPower = values->getValue<float>(VALUE_BUYING_POWER);

      SkillBranch::Ptr branch = skillsContainer->getBranchByName("banks");
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

      values->getContainer()->step();

      int n = skillsContainer->getNumBranches();
      for (int i = 0; i < n; i++)
      {
        if (!m_skillBranches.oActivated[i]->get() && values->getBranchValue<float>(VALUE_PROPAGATED, i)->full())
          m_skillBranches.oActivated[i]->set(true);
      }*/
    }

    /*void affectNeighbors()
    {
      int n = skillsContainer->getNumBranches();
      for (int i = 0; i < n; i++)
      {
        if (m_skillBranches.oActivated[i]->get())
        {
          float propagation = values->getBranchValue<float>(VALUE_PROPAGATION, i)->getValue();

          for (Neighbor::List::iterator it = m_neighbors.begin(); it != m_neighbors.end(); ++it)
          {
            if (!it->country->getSkillBranchActivated(i))
              it->country->getValues()->getBranchValue<float>(VALUE_PROPAGATED, i)->prepare(propagation, METHOD_ADD_INFLUENCE);
          }
        }
      }
    }*/
  };

  CountryData::CountryData(
    osg::ref_ptr<osgGaming::PropertiesManager> propertiesManager,
    osg::ref_ptr<SkillsContainer> skillsContainer,
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
      skillsContainer,
      name,
      id,
      population,
      wealth,
      centerLatLong,
      size))
  {
    /*int n = m->skillsContainer->getNumBranches();
    for (int i = 0; i < n; i++)
      m->m_skillBranches.oActivated[i] = new osgGaming::InitializedObservable<bool, false>();*/

    setUpdateCallback(new Callback());
  }

  CountryData::~CountryData()
  {
  }

  void CountryData::addNeighbor(Neighbor neighbor)
  {
    m->m_neighbors.push_back(neighbor);
  }

  string CountryData::getCountryName()
  {
    return m->name;
  }

  int CountryData::getId()
  {
    return m->id;
  }

  Vec2f CountryData::getCenterLatLong()
  {
    return m->centerLatLong;
  }

  Vec2f CountryData::getSize()
  {
    return m->size;
  }

  Vec2f CountryData::getSurfaceSize()
  {
    return Vec2f(
      2.0f * C_PI * sin(C_PI / 2.0f - abs(m->centerLatLong.x())) * m->earthRadius * m->size.x(),
      C_PI * m->earthRadius * m->size.y());
  }

  float CountryData::getOptimalCameraDistance(float angle, float ratio)
  {
    Vec2f surfaceSize = getSurfaceSize();

    float hdistance = surfaceSize.x() * m->cameraZoom / (2.0f * tan(angle * ratio * C_PI / 360.0f)) + m->earthRadius;
    float vdistance = surfaceSize.y() * m->cameraZoom / (2.0f * tan(angle * C_PI / 360.0f)) + m->earthRadius;

    return max(hdistance, vdistance);
  }

  CountryValues::Ptr CountryData::getValues()
  {
    return m->values;
  }

  bool CountryData::callback(SimulationVisitor* visitor)
  {/*
    if (visitor->getType() == SimulationVisitor::PROGRESS_COUNTRIES)
    {
      m->step();
      return false; // don't traverse
    }

    if (visitor->getType() == SimulationVisitor::AFFECT_NEIGHBORS)
    {
      //m->affectNeighbors();
      return false; // don't traverse
    }

    visitor->setActivatedBranches(&m->m_skillBranches);
    visitor->setCountryValues(m->values);

    return true;*/

    return true;
  }

}
