#include "CountryOverlay.h"

#include "core/Multithreading.h"
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
#include <osg/MatrixTransform>
#include <osg/Texture2D>

#include <osg/ShapeDrawable>

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
      modelContainer(injector.inject<ModelContainer>()),
      oSelectedCountryId(new osgGaming::Observable<int>(0)),
      oCurrentOverlayBranchId(new osgGaming::Observable<std::string>("")),
      hoveredCountryId(0)
  {
    initializeNodeSwitch(switchCountryNodes,      NodeSwitchType::Transparent, 0);
    initializeNodeSwitch(switchCountryHoverNodes, NodeSwitchType::Transparent, 1);
    initializeNodeSwitch(switchBoundariesMesh,    NodeSwitchType::Transparent, 2);
    initializeNodeSwitch(switchCountryPresenters, NodeSwitchType::Opaque     , 20);

    switchBoundariesMesh->addChild(0, injector.inject<BoundariesMesh>());
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
                  const osg::ref_ptr<osg::DrawElementsUInt>& triangles, CountryNode::BorderIdMap& neighborBorders,
                  const osg::ref_ptr<osg::Vec3Array>& vertices, const osg::ref_ptr<osg::Vec2Array>& texcoords1,
                  const osg::ref_ptr<osg::Vec3Array>& texcoords2)
  {
    if (switchCountryNodes->hasChild(id))
    {
      return;
    }

    auto country = modelContainer->getModel()->getCountriesTable()->getCountryById(id);
    assert_return(country);

    CountryNode::Ptr node(new CountryNode(configManager, lua, country->getName(), centerLatLong, size, vertices,
                                          texcoords1, texcoords2, triangles, neighborBorders));

    CountryHoverNode::Ptr hoverNode(new CountryHoverNode(vertices, texcoords1, triangles));

    switchCountryNodes->addChild(id, node, true);
    switchCountryHoverNodes->addChild(id, hoverNode, false);
    switchCountryPresenters->addChild(id, new CountryPresenter(), true);
  }

  CountryOverlay* base;

  osg::ref_ptr<osgGaming::ResourceManager> resourceManager;
  osg::ref_ptr<osgGaming::TextureFactory>  textureFactory;
  osg::ref_ptr<osgGaming::ShaderFactory>   shaderFactory;
  osg::ref_ptr<LuaConfig>                  configManager;
  osg::ref_ptr<LuaStateManager>            lua;

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

void CountryOverlay::loadCountries(const std::string& countriesFilename, const std::string& distanceMapFilename,
                                   const osg::ref_ptr<osg::Vec3Array>& vertices,
                                   const osg::ref_ptr<osg::Vec2Array>& texcoords)
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
  auto                         radius = m->configManager->getNumber<float>("earth.radius");
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
    auto id      = stream.read<int>();
    auto centerX = stream.read<float>();
    auto centerY = stream.read<float>();
    auto width   = stream.read<float>();
    auto height  = stream.read<float>();

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
    auto                     neighborBorderCount = stream.read<int>();
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

    osg::ref_ptr<osg::DrawElementsUInt> triangles(new osg::DrawElementsUInt(GL_TRIANGLES, 0));
    auto                                triangles_count = stream.read<int>();
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

  for (const auto& countryNode : m->switchCountryNodes->getNodes())
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

int CountryOverlay::getCountryId(const osg::Vec2f& coord) const
{
  auto mapSize = m->countriesMap->getSize();

  auto ix = static_cast<int>(coord.x() * mapSize.x());
  auto iy = static_cast<int>(coord.y() * mapSize.y());

  return m->countriesMap->getDataAt(ix, iy);
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

void CountryOverlay::setCountryIndicatorNode(int cid, osg::ref_ptr<osg::Node> node)
{
  if (!m->switchCountryPresenters->hasChild(cid))
  {
    OSGG_QLOG_WARN(QString("Country with id %1 not found.").arg(cid));
    return;
  }

  auto presenter = m->switchCountryPresenters->getChild(cid);
  if (presenter->getNumChildren() > 0)
  {
    removeCountryIndicatorNode(cid);
  }

  auto cnode   = m->switchCountryNodes->getChild(cid);
  auto latLong = cnode->getCenterLatLong();
  auto radius  = m->configManager->getNumber<float>("earth.radius");

  auto matRot =
          osg::Matrix::rotate(osgGaming::getQuatFromEuler(-latLong.x(), 0.0f, fmodf(latLong.y() + C_PI, C_PI * 2.0f)));

  auto matMove = osg::Matrix::translate(osg::Vec3f(0.0f, -radius, 0.0f));

  osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
  transform->setMatrix(matMove * matRot);

  presenter->addChild(transform);
  transform->addChild(node);
}

void CountryOverlay::removeCountryIndicatorNode(int cid)
{
  if (!m->switchCountryPresenters->hasChild(cid))
  {
    OSGG_QLOG_WARN(QString("Country with id %1 has no indicator node set").arg(cid));
  }

  auto presenter = m->switchCountryPresenters->getChild(cid);
  const auto n = presenter->getNumChildren();
  for (auto i=0U; i<n; i++)
  {
    presenter->removeChild(presenter->getChild(0));
  }
}

CountryNode* CountryOverlay::luaGetCountryNode(int id) const
{
  return getCountryNode(id).get();
}
}  // namespace onep
