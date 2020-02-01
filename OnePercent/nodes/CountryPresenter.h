#pragma once

#include "scripting/LuaBridgeDefinition.h"
#include "scripting/LuaConfig.h"

#include <osg/Group>

namespace onep
{
  class CountriesMap;
  class BoundariesData;

  class CountryPresenter : public osg::Group
  {
  public:
    class Definition : public LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    using Ptr = osg::ref_ptr<CountryPresenter>;
    using Map = std::map<int, Ptr>;

    CountryPresenter(int id, const LuaConfig::Ptr& configManager, const std::shared_ptr<CountriesMap>& countriesMap,
                     const osg::ref_ptr<BoundariesData>& boundariesMesh, const osg::Vec2f& centerLatLong,
                     const osg::Vec2f& size);

    ~CountryPresenter();

    osg::Vec2f getCenterLatLong() const;
    osg::Vec2f getSize() const;
    osg::Vec2f getSurfaceSize() const;
    float      getOptimalCameraDistance(float angle, float ratio) const;

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
    osg::ref_ptr<BoundariesData>                  m_boundariesMesh;

    float getSurfaceArea() const;

    void addTransformFromNodeToCoords(osg::Node* node, const std::string& nodeBin, const osg::Vec2f& latLong);
    void addTransformFromNodeToCenter(osg::Node* node, const std::string& nodeBin, const osg::Vec2f& relPosition);
  };
}
