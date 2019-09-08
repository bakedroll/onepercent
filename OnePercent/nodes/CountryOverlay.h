#pragma once

#include <osgGaming/Injector.h>
#include <osgGaming/Observable.h>

#include "data/CountriesMap.h"
#include "nodes/CountryNode.h"

#include <memory>

namespace onep
{
  class CountryOverlay : public LuaVisualOsgNode<osg::Group>
  {
  public:
    class Definition : public LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    using OSelectedCountryId        = osgGaming::Observable<int>;
    using OCurrentOverlayBranchName = osgGaming::Observable<std::string>;

    using Ptr = osg::ref_ptr<CountryOverlay>;

    using NeighborList = std::vector<int>;
    using NeighbourMap = std::map<int, NeighborList>;

    CountryOverlay(osgGaming::Injector& injector);
    ~CountryOverlay();

    void loadCountries(
      const std::string& countriesFilename,
      const std::string& distanceMapFilename,
      const osg::ref_ptr<osg::Vec3Array>& vertices,
      const osg::ref_ptr<osg::Vec2Array>& texcoords);

    void setHoveredCountryId(int id);

    const CountryNode::Map& getCountryNodes() const;
    CountriesMap::Ptr       getCountriesMap() const;
    const NeighbourMap&     getNeighbourships() const;

    CountryNode::Ptr getSelectedCountryNode() const;
    CountryNode::Ptr getCountryNode(int id) const;
    CountryNode::Ptr getCountryNode(const osg::Vec2f& coord) const;
    int              getCountryId(const osg::Vec2f& coord) const;

    void                    setSelectedCountry(int countryId);
    int                     getSelectedCountryId() const;
    OSelectedCountryId::Ptr getOSelectedCountryId() const;

    std::string                    getCurrentOverlayBranchName() const;
    void                           setCurrentOverlayBranchName(const std::string& branchName);
    OCurrentOverlayBranchName::Ptr getOCurrentOverlayBranchName() const;

    void setCountryIndicatorNode(int cid, osg::ref_ptr<osg::Node> node);
    void removeCountryIndicatorNode(int cid);

    CountryNode* luaGetCountryNode(int id) const;

private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}
