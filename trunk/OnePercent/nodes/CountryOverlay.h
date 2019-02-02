#pragma once

#include <osgGaming/Injector.h>
#include <osgGaming/Observable.h>

#include "data/CountriesMap.h"
#include "nodes/CountryNode.h"

#include <memory>

namespace onep
{
  class CountryOverlay : public osg::Group
  {
  public:
    class Definition : public LuaClassDefinition
    {
    public:
      void registerClass(lua_State* state) override;
    };

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
    CountryNode::Ptr getCountryNode(int id) const;
    CountryNode::Ptr getCountryNode(osg::Vec2f coord);
    int getCountryId(osg::Vec2f coord);

    int getSelectedCountryId();
    osgGaming::Observable<int>::Ptr getSelectedCountryIdObservable();

    CountryNode* luaGetCountryNode(int id) const;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}
