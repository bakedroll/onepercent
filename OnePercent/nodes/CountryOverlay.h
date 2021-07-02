#pragma once

#include <osgHelper/ioc/Injector.h>
#include <osgHelper/Observable.h>

#include "data/CountriesMap.h"
#include "nodes/CountryNode.h"
#include "nodes/CountryPresenter.h"

#include <memory>

namespace onep
{
  class CountryOverlay : public LuaVisualOsgNode<osg::Group>
  {
  public:
    class Definition : public luaHelper::LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    using OSelectedCountryId        = osgHelper::Observable<int>;
    using OCurrentOverlayBranchName = osgHelper::Observable<std::string>;

    using Ptr = osg::ref_ptr<CountryOverlay>;

    using NeighborList = std::vector<int>;
    using NeighbourMap = std::map<int, NeighborList>;

    CountryOverlay(osgHelper::ioc::Injector& injector);
    ~CountryOverlay();

    void loadCountries(const std::string& countriesFilename, const std::string& distanceMapFilename);

    void setHoveredCountryId(int id);

    const CountryNode::Map&      getCountryNodes() const;
    const CountryPresenter::Map& getCountryPresenters() const;

    CountriesMap::Ptr   getCountriesMap() const;
    const NeighbourMap& getNeighbourships() const;

    CountryNode::Ptr      getSelectedCountryNode() const;
    CountryNode::Ptr      getCountryNode(int id) const;
    CountryNode::Ptr      getCountryNode(const osg::Vec2f& coord) const;
    CountryPresenter::Ptr getCountryPresenter(int id) const;
    int                   getCountryId(const osg::Vec2f& coord) const;

    void                    setSelectedCountry(int countryId);
    int                     getSelectedCountryId() const;
    OSelectedCountryId::Ptr getOSelectedCountryId() const;

    std::string                    getCurrentOverlayBranchName() const;
    void                           setCurrentOverlayBranchName(const std::string& branchName);
    OCurrentOverlayBranchName::Ptr getOCurrentOverlayBranchName() const;

    CountryNode*      luaGetCountryNode(int id) const;
    CountryPresenter* luaGetCountryPresenter(int id) const;
    luabridge::LuaRef luaGetCountryPresenters() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}
