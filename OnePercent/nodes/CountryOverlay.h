#pragma once

#include "simulation/SkillBranch.h"
#include "data/CountriesMap.h"
#include "nodes/CountryMesh.h"

#include <osg/Switch>

#include <memory>

namespace onep
{
  class CountryOverlay : public osg::Switch
  {
  public:
    typedef osg::ref_ptr<CountryOverlay> Ptr;

    CountryOverlay();
    ~CountryOverlay();

    void loadCountries(
      std::string countriesFilename,
      std::string distanceMapFilename,
      osg::ref_ptr<osg::Vec3Array> vertices,
      osg::ref_ptr<osg::Vec2Array> texcoords);

    void clearHighlightedCountries();
    void setSelectedCountry(int countryId);
    void setHighlightedSkillBranch(BranchType type);

    void setHoveredCountry(CountryMesh::Ptr countryMesh);
    void setAllCountriesVisibility(bool visibility);

    CountryMesh::Map& getCountryMeshs();
    CountriesMap::Ptr getCountriesMap();

    CountryMesh::Ptr getSelectedCountryMesh();
    CountryMesh::Ptr getCountryMesh(int id);
    CountryMesh::Ptr getCountryMesh(osg::Vec2f coord);
    int getCountryId(osg::Vec2f coord);
    std::string getCountryName(osg::Vec2f coord);

    int getSelectedCountryId();
    osgGaming::Observable<int>::Ptr getSelectedCountryIdObservable();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}
