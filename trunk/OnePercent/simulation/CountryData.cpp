#include "CountryData.h"

#include "core/Globals.h"

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
      , populationInMio(population)
      , id(id)
      , centerLatLong(centerLatLong)
      , size(size)
      , earthRadius(propertiesManager->getValue<float>(Param_EarthRadiusName))
      , cameraZoom(propertiesManager->getValue<float>(Param_CameraCountryZoomName))
    {}

    std::string name;

    SkillsContainer::Ptr skillsContainer;
    Neighbor::List m_neighbors;

    float populationInMio;

    int id;
    osg::Vec2f centerLatLong;
    osg::Vec2f size;

    float earthRadius;
    float cameraZoom;

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
    : osg::Referenced()
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

}
