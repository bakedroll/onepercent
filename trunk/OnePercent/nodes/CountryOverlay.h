#pragma once

#include <osgGaming/Injector.h>

#include "simulation/SkillBranch.h"
#include "data/CountriesMap.h"
#include "nodes/CountryNode.h"

#include <memory>

namespace onep
{
  class CountryOverlay : public osg::Group
  {
  public:
    typedef osg::ref_ptr<CountryOverlay> Ptr;

    typedef std::vector<int> NeighborList;
    typedef std::map<int, NeighborList> NeighbourMap;

    CountryOverlay(osgGaming::Injector& injector);
    ~CountryOverlay();

    void loadCountries(
      std::string countriesFilename,
      std::string distanceMapFilename,
      osg::ref_ptr<osg::Vec3Array> vertices,
      osg::ref_ptr<osg::Vec2Array> texcoords);

    void clearHighlightedCountries();
    void setSelectedCountry(int countryId);
    void setHighlightedSkillBranch(int id);

    void setHoveredCountryId(int id);
    void setAllCountriesVisibility(bool visibility);

    CountryNode::Map& getCountryNodes();
    CountriesMap::Ptr getCountriesMap();
    NeighbourMap& getNeighbourships();

    CountryNode::Ptr getSelectedCountryNode();
    CountryNode::Ptr getCountryNode(int id);
    CountryNode::Ptr getCountryNode(osg::Vec2f coord);
    int getCountryId(osg::Vec2f coord);

    int getSelectedCountryId();
    osgGaming::Observable<int>::Ptr getSelectedCountryIdObservable();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}
