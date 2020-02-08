#include "CountryOverlay.h"

#include "core/Multithreading.h"
#include "data/BoundariesData.h"
#include "nodes/BoundariesMesh.h"
#include "nodes/CountryHoverNode.h"
#include "nodes/CountryPresenter.h"
#include "nodes/NodeSwitch.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaConfig.h"
#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaCountry.h"
#include "scripting/LuaModel.h"
#include "simulation/ModelContainer.h"

#include <osgGaming/ByteStream.h>
#include <osgGaming/Helper.h>
#include <osgGaming/Observable.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/ShaderFactory.h>
#include <osgGaming/TextureFactory.h>

#include <osg/BlendFunc>
#include <osg/Texture2D>

namespace onep
{
enum class NodeSwitchType
{
  Transparent,
  Opaque
};

using StateSetPtr  = osg::ref_ptr<osg::StateSet>;
using Texture2DPtr = osg::ref_ptr<osg::Texture2D>;

void setStateSetTexture(const StateSetPtr& stateSet, const Texture2DPtr& texture, const char* uniformName)
{
  stateSet->addUniform(new osg::Uniform(uniformName, 0));
  stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
}

void CountryOverlay::Definition::registerDefinition(lua_State* state)
{
  getGlobalNamespace(state)
          .deriveClass<CountryOverlay, LuaVisualOsgNode<osg::Group>>("CountryOverlay")
          .addFunction("get_country_node", &CountryOverlay::luaGetCountryNode)
          .addFunction("set_current_overlay_branch_name", &CountryOverlay::setCurrentOverlayBranchName)
          .addFunction("get_country_presenter", &CountryOverlay::luaGetCountryPresenter)
          .addFunction("get_country_presenters", &CountryOverlay::luaGetCountryPresenters)
          .addFunction("get_current_country_id", &CountryOverlay::getSelectedCountryId)
          .endClass();
}

struct CountryOverlay::Impl
{
  Impl(osgGaming::Injector& injector, CountryOverlay* b)
    : base(b),
      resourceManager(injector.inject<osgGaming::ResourceManager>()),
      textureFactory(injector.inject<osgGaming::TextureFactory>()),
      shaderFactory(injector.inject<osgGaming::ShaderFactory>()),
      configManager(injector.inject<LuaConfig>()),
      lua(injector.inject<LuaStateManager>()),
      boundariesData(injector.inject<BoundariesData>()),
      boundariesMesh(injector.inject<BoundariesMesh>()),
      modelContainer(injector.inject<ModelContainer>()),
      countriesMap(std::make_shared<CountriesMap>()),
      oSelectedCountryId(new osgGaming::Observable<int>(0)),
      oCurrentOverlayBranchId(new osgGaming::Observable<std::string>("")),
      hoveredCountryId(0)
  {
    initializeNodeSwitch(switchCountryNodes, NodeSwitchType::Transparent, 0);
    initializeNodeSwitch(switchCountryHoverNodes, NodeSwitchType::Transparent, 1);
    initializeNodeSwitch(switchBoundariesMesh, NodeSwitchType::Transparent, 2);
    initializeNodeSwitch(switchCountryPresenters, NodeSwitchType::Opaque, 20);

    switchBoundariesMesh->addChild(0, boundariesMesh);
  }

  template <typename TNode>
  void initializeNodeSwitch(osg::ref_ptr<NodeSwitch<int, TNode>>& nodeSwitch, NodeSwitchType type, int renderBin)
  {
    nodeSwitch = new NodeSwitch<int, TNode>();

    auto stateSet = nodeSwitch->getOrCreateStateSet();
    if (type == NodeSwitchType::Opaque)
    {
      stateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
      stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
      stateSet->setMode(GL_BLEND, osg::StateAttribute::OFF);
    }
    else
    {
      stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
      stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
      stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    }

    stateSet->setRenderBinDetails(renderBin, "RenderBin");

    base->addChild(nodeSwitch);
  }

  void addCountry(int id, const osg::Vec2f& centerLatLong, const osg::Vec2f& size,
                  const osg::ref_ptr<osg::DrawElementsUInt>& triangles, BoundariesData::BorderIdMap& neighborBorders,
                  const osg::ref_ptr<osg::Vec3Array>& vertices, const osg::ref_ptr<osg::Vec2Array>& texcoordsPolar,
                  const osg::ref_ptr<osg::Vec3Array>& texcoordsCartesian,
                  const osg::ref_ptr<osg::Vec2Array>& texcoordsProjected)
  {
    if (switchCountryNodes->hasChild(id))
    {
      return;
    }

    const auto country = modelContainer->getModel()->getCountriesTable()->getCountryById(id);
    assert_return(country);

    switchCountryNodes->addChild(id,
                                 new CountryNode(lua, country->getName(), vertices, texcoordsPolar, texcoordsCartesian,
                                                 texcoordsProjected, triangles),
                                 true);

    switchCountryHoverNodes->addChild(id, new CountryHoverNode(vertices, texcoordsPolar, triangles), false);
    switchCountryPresenters->addChild(
            id,
            new CountryPresenter(id, configManager, countriesMap, boundariesData, centerLatLong, size, neighborBorders),
            true);
  }

  CountryOverlay* base;

  osg::ref_ptr<osgGaming::ResourceManager> resourceManager;
  osg::ref_ptr<osgGaming::TextureFactory>  textureFactory;
  osg::ref_ptr<osgGaming::ShaderFactory>   shaderFactory;
  osg::ref_ptr<LuaConfig>                  configManager;
  osg::ref_ptr<LuaStateManager>            lua;
  osg::ref_ptr<BoundariesData>             boundariesData;
  osg::ref_ptr<BoundariesMesh>             boundariesMesh;

  ModelContainer::Ptr modelContainer;

  osg::ref_ptr<NodeSwitch<int, CountryNode>>      switchCountryNodes;
  osg::ref_ptr<NodeSwitch<int, CountryHoverNode>> switchCountryHoverNodes;
  osg::ref_ptr<NodeSwitch<int, BoundariesMesh>>   switchBoundariesMesh;
  osg::ref_ptr<NodeSwitch<int, CountryPresenter>> switchCountryPresenters;

  CountriesMap::Ptr countriesMap;

  OSelectedCountryId::Ptr        oSelectedCountryId;
  OCurrentOverlayBranchName::Ptr oCurrentOverlayBranchId;

  int hoveredCountryId;

  NeighbourMap neighbourMap;

  QMutex currentBranchIdMutex;
  QMutex selectedCountryMutex;
};

CountryOverlay::CountryOverlay(osgGaming::Injector& injector)
  : LuaVisualOsgNode<osg::Group>(), m(new Impl(injector, this))
{
  addStateSetUniform(new osg::Uniform("takeoverColor", osg::Vec4f(0.0f, 0.0f, 0.0f, 0.0f)), m->switchCountryNodes);
  addStateSetUniform(new osg::Uniform("takeoverScale", 100.0f), m->switchCountryNodes);
}

CountryOverlay::~CountryOverlay()
{
  for (const auto& node : m->switchCountryNodes->getNodes())
  {
    node.second->clearNeighbors();
  }
}

void CountryOverlay::loadCountries(const std::string& countriesFilename, const std::string& distanceMapFilename)
{
  // Load shaders
  osg::ref_ptr<osg::Program> cnProgram(new osg::Program());
  osg::ref_ptr<osg::Program> chnProgram(new osg::Program());

  auto frag_shader = m->shaderFactory->make()
                             ->type(osg::Shader::FRAGMENT)
                             ->module(m->resourceManager->loadText(
                                     QString("./GameData/shaders/modules/%1.glsl").arg("noise3D").toStdString()))
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
  auto distanceTexture = m->textureFactory->make()->image(m->resourceManager->loadImage(distanceMapFilename))->build();

  // Create statesets
  auto csStateSet = m->switchCountryNodes->getOrCreateStateSet();
  csStateSet->setAttributeAndModes(cnProgram, osg::StateAttribute::ON);
  setStateSetTexture(csStateSet, distanceTexture, "distanceMap");
  csStateSet->setAttributeAndModes(new osg::BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);

  auto chsStateSet = m->switchCountryHoverNodes->getOrCreateStateSet();
  chsStateSet->setAttributeAndModes(chnProgram, osg::StateAttribute::ON);
  setStateSetTexture(chsStateSet, distanceTexture, "distanceMap");

  m->neighbourMap.clear();

  // Calculate 2nd texcoord layer
  auto       radius = m->configManager->getNumber<float>("earth.radius");
  const auto bytes  = m->resourceManager->loadBinary(countriesFilename);

  osgGaming::ByteStream stream(bytes);

  const auto ncountries = stream.read<int>();
  for (auto i = 0; i < ncountries; i++)
  {
    auto id      = stream.read<int>();
    auto centerX = stream.read<float>();
    auto centerY = stream.read<float>();
    auto width   = stream.read<float>();
    auto height  = stream.read<float>();

    osg::Vec2f centerLatLong((0.5f - centerY) * C_PI, fmodf(centerX + 0.5f, 1.0f) * 2.0f * C_PI);
    osg::Vec2f size(width, height);

    NeighborList neighborList;

    auto neighborsCount = stream.read<int>();
    for (auto j = 0; j < neighborsCount; j++)
    {
      auto neighbourId = stream.read<int>();
      neighborList.push_back(neighbourId);
    }

    m->neighbourMap.insert(NeighbourMap::value_type(id, neighborList));

    BoundariesData::BorderIdMap neighborBorderMap;
    auto                        neighborBorderCount = stream.read<int>();
    for (auto j = 0; j < neighborBorderCount; j++)
    {
      std::vector<int> borders;

      auto nid    = stream.read<int>();
      auto bcount = stream.read<int>();
      for (auto k = 0; k < bcount; k++)
      {
        borders.push_back(stream.read<int>());
      }

      neighborBorderMap[nid] = borders;
    }

    const auto numVerts = stream.read<int>();

    auto vertices           = new osg::Vec3Array();
    auto texcoordsPolar     = new osg::Vec2Array();
    auto texcoordsCartesian = new osg::Vec3Array();
    auto texcoordsProjected = new osg::Vec2Array();

    vertices->reserve(numVerts);
    texcoordsPolar->reserve(numVerts);
    texcoordsCartesian->reserve(numVerts);

    for (auto j=0; j<numVerts; j++)
    {
      const auto x = stream.read<float>();
      const auto y = stream.read<float>();
      const auto z = stream.read<float>();

      const auto u = stream.read<float>();
      const auto v = stream.read<float>();

      const auto cu = stream.read<float>();
      const auto cv = stream.read<float>();
      const auto cw = stream.read<float>();

      const auto pu = stream.read<float>();
      const auto pv = stream.read<float>();

 
      vertices->push_back(osg::Vec3f(x, y, z));
      texcoordsPolar->push_back(osg::Vec2f(u, v));
      texcoordsCartesian->push_back(osg::Vec3f(cu, cv, cw));
      texcoordsProjected->push_back(osg::Vec2f(pu, pv));
    }

    osg::ref_ptr<osg::DrawElementsUInt> triangles(new osg::DrawElementsUInt(GL_TRIANGLES, 0));
    const auto                          numTriangles = stream.read<int>();
    for (auto j = 0; j < numTriangles; j++)
    {
      const auto v0 = stream.read<int>();
      const auto v1 = stream.read<int>();
      const auto v2 = stream.read<int>();

      triangles->push_back(v0);
      triangles->push_back(v2);
      triangles->push_back(v1);
    }

    m->addCountry(id, centerLatLong, size, triangles, neighborBorderMap, vertices, texcoordsPolar, texcoordsCartesian,
                  texcoordsProjected);
  }

  for (const auto& countryNode : m->switchCountryNodes->getNodes())
  {
    NeighborList neighbours = m->neighbourMap.find(countryNode.first)->second;

    for (const auto& neighbour : neighbours)
    {
      countryNode.second->addNeighbor(getCountryNode(neighbour));
    }
  }

  const auto mapWidth  = stream.read<int>();
  const auto mapHeight = stream.read<int>();

  m->countriesMap->initialize(mapWidth, mapHeight, reinterpret_cast<unsigned char*>(&bytes[stream.getPos()]));
  m->boundariesMesh->makeOverallBoundaries(0.005f);
}

void CountryOverlay::setHoveredCountryId(int id)
{
  if (id == m->hoveredCountryId)
  {
    return;
  }

  if (m->hoveredCountryId > 0)
  {
    m->switchCountryHoverNodes->setKeyValue(m->hoveredCountryId, false);
    m->switchCountryHoverNodes->getChild(m->hoveredCountryId)->setHoverEnabled(false);
  }

  if (id > 0)
  {
    m->switchCountryHoverNodes->setKeyValue(id, true);
    m->switchCountryHoverNodes->getChild(id)->setHoverEnabled(true);
  }

  m->hoveredCountryId = id;
}

const CountryNode::Map& CountryOverlay::getCountryNodes() const
{
  return m->switchCountryNodes->getNodes();
}

const CountryPresenter::Map& CountryOverlay::getCountryPresenters() const
{
  return m->switchCountryPresenters->getNodes();
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
  return getCountryNode(m->oSelectedCountryId->get());
}

CountryNode::Ptr CountryOverlay::getCountryNode(int id) const
{
  return m->switchCountryNodes->getChild(id);
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

CountryPresenter::Ptr CountryOverlay::getCountryPresenter(int id) const
{
  return m->switchCountryPresenters->getChild(id);
}

int CountryOverlay::getCountryId(const osg::Vec2f& coord) const
{
  return m->countriesMap->getDataAt(coord);
}

void CountryOverlay::setSelectedCountry(int countryId)
{
  Multithreading::executeInUiAsync([this, countryId]() {
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
  Multithreading::executeInUiAsync([this, branchName]() {
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
  return getCountryNode(id);
}

CountryPresenter* CountryOverlay::luaGetCountryPresenter(int id) const
{
  return getCountryPresenter(id);
}

luabridge::LuaRef CountryOverlay::luaGetCountryPresenters() const
{
  auto  table = m->lua->newTable();
  const auto& nodes = m->switchCountryPresenters->getNodes();
  for (const auto& node : nodes)
  {
    table[node.first] = node.second.get();
  }

  return table;
}
}  // namespace onep
