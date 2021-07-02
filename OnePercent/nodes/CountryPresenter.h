#pragma once

#include "data/BoundariesData.h"

#include "scripting/LuaConfig.h"

#include <osg/Switch>

#include <luaHelper/LuaBridgeDefinition.h>

namespace onep
{
  class CountriesMap;
  class BoundariesData;

  class CountryPresenter : public osg::Switch
  {
  public:
    class Definition : public luaHelper::LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    using Ptr = osg::ref_ptr<CountryPresenter>;
    using Map = std::map<int, Ptr>;

    CountryPresenter(int id, const LuaConfig::Ptr& configManager, const std::shared_ptr<CountriesMap>& countriesMap,
                     const osg::ref_ptr<BoundariesData>& boundariesData, const osg::Vec2f& centerLatLong,
                     const osg::Vec2f& size, const BoundariesData::BorderIdMap& neighbourBorders);

    ~CountryPresenter();

    osg::Vec2f                         getCenterLatLong() const;
    osg::Vec2f                         getSize() const;
    osg::Vec2f                         getSurfaceSize() const;
    float                              getOptimalCameraDistance(float angle, float ratio) const;
    const BoundariesData::BorderIdMap& getNeighborBorders() const;
    bool                               getIsOnOcean() const;

    void luaMakeBoundaries(const osg::Vec3f& color, float thickness);
    void luaSetBoundariesEnabled(bool enabled);

    void luaAddNode(osg::Node* node);
    void luaAddNodeToBin(osg::Node* node, const std::string& nodeBin);
    void luaAddNodeToBinAt(osg::Node* node, const std::string& nodeBin, const osg::Vec2f& relPosition);
    void luaScatterNodesToBin(osg::Node* node, const std::string& nodeBin, float density);
    void luaRemoveNodeBin(const std::string& nodeBin);
    void luaClearNodes();

private:
    using MatrixTransformPtr     = osg::ref_ptr<osg::MatrixTransform>;
    using MatrixTransformPtrList = std::vector<MatrixTransformPtr>;

    int        m_id;
    osg::Vec2f m_centerLatLong;
    osg::Vec2f m_size;
    float      m_earthRadius;
    float      m_cameraZoom;

    std::map<std::string, MatrixTransformPtrList> m_transformBins;
    std::shared_ptr<CountriesMap>                 m_countriesMap;
    osg::ref_ptr<BoundariesData>                  m_boundariesData;
    osg::ref_ptr<osg::Geode>                      m_boundsMesh;
    osg::ref_ptr<osg::Group>                      m_nodes;

    BoundariesData::BorderIdMap m_neighbourBorders;

    osg::ref_ptr<osg::Group>& getOrCreateNodesGroup();
    float                     getSurfaceArea() const;

    void addTransformFromNodeToCoords(osg::Node* node, const std::string& nodeBin, const osg::Vec2f& latLong);
    void addTransformFromNodeToCenter(osg::Node* node, const std::string& nodeBin, const osg::Vec2f& relPosition);
  };
}
