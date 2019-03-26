#include "CountryOverlay.h"

#include "core/Multithreading.h"
#include "nodes/CountryHoverNode.h"
#include "simulation/ModelContainer.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaConfig.h"
#include "scripting/LuaCountry.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaCountriesTable.h"

#include <osgGaming/Observable.h>
#include <osgGaming/ByteStream.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/TextureFactory.h>
#include <osgGaming/ShaderFactory.h>
#include <osgGaming/Helper.h>

#include <osg/Switch>
#include <osg/Texture2D>
#include <osg/BlendFunc>

namespace onep
{
  void CountryOverlay::Definition::registerClass(lua_State* state)
  {
    getGlobalNamespace(state)
      .deriveClass<CountryOverlay, LuaVisualOsgNode<osg::Group>>("CountryOverlay")
      .addFunction("get_country_node", &CountryOverlay::luaGetCountryNode)
      .addFunction("set_current_overlay_branch_name", &CountryOverlay::setCurrentOverlayBranchName)
      .endClass();
  }

  struct CountryOverlay::Impl
  {
    Impl(osgGaming::Injector& injector, CountryOverlay* b)
      : base(b)
      , resourceManager(injector.inject<osgGaming::ResourceManager>())
      , textureFactory(injector.inject<osgGaming::TextureFactory>())
      , shaderFactory(injector.inject<osgGaming::ShaderFactory>())
      , configManager(injector.inject<LuaConfig>())
      , lua(injector.inject<LuaStateManager>())
      , modelContainer(injector.inject<ModelContainer>())
      , countrySwitch(new osg::Switch())
      , countryHoverSwitch(new osg::Switch())
      , oSelectedCountryId(new osgGaming::Observable<int>(0))
      , oCurrentOverlayBranchId(new osgGaming::Observable<std::string>(""))
    {
    }

    void addCountry(
      int id,
      const osg::Vec2f& centerLatLong,
      const osg::Vec2f& size,
      const osg::ref_ptr<osg::DrawElementsUInt>& triangles,
      CountryNode::BorderIdMap& neighborBorders,
      const osg::ref_ptr<osg::Vec3Array>& vertices,
      const osg::ref_ptr<osg::Vec2Array>& texcoords1,
      const osg::ref_ptr<osg::Vec3Array>& texcoords2)
    {
      if (countryNodes.find(id) != countryNodes.end())
      {
        return;
      }

      auto country = modelContainer->getModel()->getCountriesTable()->getCountryById(id);
      assert_return(country);

      CountryNode::Ptr node(new CountryNode(configManager, lua, country->getName(), centerLatLong, size, vertices,
                                            texcoords1, texcoords2, triangles, neighborBorders));

      CountryHoverNode::Ptr hoverNode(new CountryHoverNode(vertices, texcoords1, triangles));

      countryNodes.insert(CountryNode::Map::value_type(id, node));
      countrySwitch->addChild(node, true);

      countryHoverNodes.insert(CountryHoverNode::Map::value_type(id, hoverNode));
      countryHoverSwitch->addChild(hoverNode, false);
    }

    CountryOverlay* base;

    osg::ref_ptr<osgGaming::ResourceManager> resourceManager;
    osg::ref_ptr<osgGaming::TextureFactory>  textureFactory;
    osg::ref_ptr<osgGaming::ShaderFactory>   shaderFactory;
    osg::ref_ptr<LuaConfig>                  configManager;
    osg::ref_ptr<LuaStateManager>            lua;

    ModelContainer::Ptr modelContainer;

    osg::ref_ptr<osg::Switch> countrySwitch;
    osg::ref_ptr<osg::Switch> countryHoverSwitch;

    CountryNode::Map      countryNodes;
    CountryHoverNode::Map countryHoverNodes;
    CountriesMap::Ptr     countriesMap;

    OSelectedCountryId::Ptr        oSelectedCountryId;
    OCurrentOverlayBranchName::Ptr oCurrentOverlayBranchId;

    CountryHoverNode::Ptr hoveredCountryNode;

    NeighbourMap neighbourMap;

    QMutex currentBranchIdMutex;
    QMutex selectedCountryMutex;
  };

  CountryOverlay::CountryOverlay(osgGaming::Injector& injector)
    : LuaVisualOsgNode<osg::Group>()
    , m(new Impl(injector, this))
  {
    addChild(m->countrySwitch);
    addChild(m->countryHoverSwitch);

    addStateSetUniform(new osg::Uniform("takeoverColor", osg::Vec4f(0.0f, 0.0f, 0.0f, 0.0f)), m->countrySwitch);
    addStateSetUniform(new osg::Uniform("takeoverScale", 100.0f), m->countrySwitch);
  }

  CountryOverlay::~CountryOverlay() = default;

  void CountryOverlay::loadCountries(
    const std::string& countriesFilename,
    const std::string& distanceMapFilename,
    const osg::ref_ptr<osg::Vec3Array>& vertices,
    const osg::ref_ptr<osg::Vec2Array>& texcoords)
  {
    // Load shaders
    osg::ref_ptr<osg::Program> cnProgram(new osg::Program());
    osg::ref_ptr<osg::Program> chnProgram(new osg::Program());

    auto frag_shader = m->shaderFactory->make()
      ->type(osg::Shader::FRAGMENT)
      ->module(m->resourceManager->loadText(QString("./GameData/shaders/modules/%1.glsl").arg("noise3D").toStdString()))
      ->module(m->resourceManager->loadText("./GameData/shaders/country.frag"))
      ->build();

    auto frag_shader_hover = m->shaderFactory->make()
      ->type(osg::Shader::FRAGMENT)
      ->module(m->resourceManager->loadText("./GameData/shaders/countryHover.frag"))
      ->build();

    auto vert_shader = m->resourceManager->loadShader("./GameData/shaders/country.vert", osg::Shader::VERTEX);

    cnProgram->addShader(frag_shader);
    cnProgram->addShader(vert_shader);

    chnProgram->addShader(frag_shader_hover);
    chnProgram->addShader(vert_shader);

    // Load distance texture
    auto distanceTexture = m->textureFactory->make()
      ->image(m->resourceManager->loadImage(distanceMapFilename))
      ->build();

    // Create statesets
    auto stateSet = getOrCreateStateSet();

    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    stateSet->setRenderBinDetails(1, "RenderBin");
    stateSet->addUniform(new osg::Uniform("distancemap", 0));
    stateSet->setTextureAttributeAndModes(0, distanceTexture, osg::StateAttribute::ON);

    auto csStateSet = m->countrySwitch->getOrCreateStateSet();
    csStateSet->setAttributeAndModes(cnProgram, osg::StateAttribute::ON);
    csStateSet->setAttributeAndModes(new osg::BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);
    csStateSet->setRenderBinDetails(0, "RenderBin");

    auto chsStateSet = m->countryHoverSwitch->getOrCreateStateSet();
    chsStateSet->setAttributeAndModes(chnProgram, osg::StateAttribute::ON);
    chsStateSet->setRenderBinDetails(1, "RenderBin");

    m->neighbourMap.clear();

    // Calculate 2nd texcoord layer
    auto radius = m->configManager->getNumber<float>("earth.radius");
    osg::ref_ptr<osg::Vec3Array> texcoords2(new osg::Vec3Array());
    for (const auto& vertex : *vertices)
    {
      texcoords2->push_back((vertex / radius + osg::Vec3f(1.0f, 1.0f, 1.0f)) / 2.0f);
    }

    auto bytes = m->resourceManager->loadBinary(countriesFilename);

    osgGaming::ByteStream stream(bytes);

    auto ncountries = stream.read<int>();
    for (auto i = 0; i < ncountries; i++)
    {
      auto id = stream.read<int>();
      auto centerX = stream.read<float>();
      auto centerY = stream.read<float>();
      auto width = stream.read<float>();
      auto height = stream.read<float>();

      osg::Vec2f centerLatLong((0.5f - centerY) * C_PI, fmodf(centerX + 0.5f, 1.0f) * 2.0f * C_PI);
      osg::Vec2f size(width, height);

      NeighborList neighborList;

      auto neighbors_count = stream.read<int>();
      for (auto j = 0; j < neighbors_count; j++)
      {
        auto neighbourId = stream.read<int>();
        neighborList.push_back(neighbourId);
      }

      m->neighbourMap.insert(NeighbourMap::value_type(id, neighborList));

      CountryNode::BorderIdMap neighborBorderMap;
      auto neighborBorderCount = stream.read<int>();
      for (auto j = 0; j < neighborBorderCount; j++)
      {
        std::vector<int> borders;

        auto nid = stream.read<int>();
        auto bcount = stream.read<int>();
        for (auto k = 0; k < bcount; k++)
        {
          borders.push_back(stream.read<int>());
        }

        neighborBorderMap[nid] = borders;
      }

      osg::ref_ptr<osg::DrawElementsUInt> triangles(new osg::DrawElementsUInt(GL_TRIANGLES, 0));
      auto triangles_count = stream.read<int>();
      for (auto j = 0; j < triangles_count; j++)
      {
        auto v0 = stream.read<int>();
        auto v1 = stream.read<int>();
        auto v2 = stream.read<int>();

        triangles->push_back(v0);
        triangles->push_back(v2);
        triangles->push_back(v1);
      }

      m->addCountry(id, centerLatLong, size, triangles, neighborBorderMap, vertices, texcoords, texcoords2);
    }

    for (const auto& countryNode : m->countryNodes)
    {
      NeighborList neighbours = m->neighbourMap.find(countryNode.first)->second;

      for (const auto& neighbour : neighbours)
      {
        countryNode.second->addNeighbor(getCountryNode(neighbour));
      }
    }

    auto mapWidth  = stream.read<int>();
    auto mapHeight = stream.read<int>();

    m->countriesMap = new CountriesMap(mapWidth, mapHeight, reinterpret_cast<unsigned char*>(&bytes[stream.getPos()]));
  }

  void CountryOverlay::setHoveredCountryId(int id)
  {
    const auto& countryHoverNode = m->countryHoverNodes.count(id) > 0 ? m->countryHoverNodes[id] : nullptr;

    if (countryHoverNode == m->hoveredCountryNode)
    {
      return;
    }

    if (m->hoveredCountryNode)
    {
      m->countryHoverSwitch->setChildValue(m->hoveredCountryNode, false);
      m->hoveredCountryNode->setHoverEnabled(false);
    }

    if (countryHoverNode)
    {
      m->countryHoverSwitch->setChildValue(countryHoverNode, true);
      countryHoverNode->setHoverEnabled(true);
    }

    m->hoveredCountryNode = countryHoverNode;
  }

  const CountryNode::Map& CountryOverlay::getCountryNodes() const
  {
    return m->countryNodes;
  }

  CountriesMap::Ptr CountryOverlay::getCountriesMap() const
  {
    return m->countriesMap;
  }

  const CountryOverlay::NeighbourMap& CountryOverlay::getNeighbourships() const
  {
    return m->neighbourMap;
  }

  CountryNode::Ptr CountryOverlay::getSelectedCountryNode() const
  {
    return m->countryNodes.find(m->oSelectedCountryId->get())->second;
  }

  CountryNode::Ptr CountryOverlay::getCountryNode(int id) const
  {
    auto countryNode = m->countryNodes.find(id);

    if (countryNode == m->countryNodes.end())
    {
      return nullptr;
    }

    return countryNode->second;
  }

  CountryNode::Ptr CountryOverlay::getCountryNode(const osg::Vec2f& coord) const
  {
    auto id = getCountryId(coord);

    if (id == 0)
    {
      return nullptr;
    }

    return getCountryNode(id);
  }

  int CountryOverlay::getCountryId(const osg::Vec2f& coord) const
  {
    auto mapSize = m->countriesMap->getSize();

    auto ix = static_cast<int>(coord.x() * mapSize.x());
    auto iy = static_cast<int>(coord.y() * mapSize.y());

    return m->countriesMap->getDataAt(ix, iy);
  }

  void CountryOverlay::setSelectedCountry(int countryId)
  {
    Multithreading::uiExecuteOrAsync([this, countryId]()
    {
      QMutexLocker lock(&m->selectedCountryMutex);
      m->oSelectedCountryId->set(countryId);
    });
  }

  int CountryOverlay::getSelectedCountryId() const
  {
    QMutexLocker lock(&m->selectedCountryMutex);
    return m->oSelectedCountryId->get();
  }

  CountryOverlay::OSelectedCountryId::Ptr CountryOverlay::getOSelectedCountryId() const
  {
    return m->oSelectedCountryId;
  }

  std::string CountryOverlay::getCurrentOverlayBranchName() const
  {
    QMutexLocker lock(&m->currentBranchIdMutex);
    return m->oCurrentOverlayBranchId->get();
  }

  void CountryOverlay::setCurrentOverlayBranchName(const std::string& branchName)
  {
    Multithreading::uiExecuteOrAsync([this, branchName]()
    {
      QMutexLocker lock(&m->currentBranchIdMutex);
      m->oCurrentOverlayBranchId->set(branchName);
    });
  }

  CountryOverlay::OCurrentOverlayBranchName::Ptr CountryOverlay::getOCurrentOverlayBranchName() const
  {
    return m->oCurrentOverlayBranchId;
  }

  CountryNode* CountryOverlay::luaGetCountryNode(int id) const
  {
    return getCountryNode(id).get();
  }
}
