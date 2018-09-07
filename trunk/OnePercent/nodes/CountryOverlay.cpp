#include "CountryOverlay.h"

#include "core/Multithreading.h"
#include "nodes/CountryHoverNode.h"
#include "scripting/ConfigManager.h"
#include "simulation/CountriesContainer.h"
#include "simulation/SimulationStateContainer.h"
#include "simulation/SimulationState.h"
#include "simulation/Country.h"

#include <osgGaming/Observable.h>
#include <osgGaming/ByteStream.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/TextureFactory.h>
#include <osgGaming/ShaderFactory.h>
#include <osgGaming/Helper.h>

#include <osg/Switch>
#include <osg/Texture2D>

namespace onep
{
  struct CountryOverlay::Impl
  {
    Impl(osgGaming::Injector& injector, CountryOverlay* b)
      : resourceManager(injector.inject<osgGaming::ResourceManager>())
      , textureFactory(injector.inject<osgGaming::TextureFactory>())
      , shaderFactory(injector.inject<osgGaming::ShaderFactory>())
      , configManager(injector.inject<ConfigManager>())
      , stateContainer(injector.inject<SimulationStateContainer>())
      , skillsContainer(injector.inject<SkillsContainer>())
      , countriesContainer(injector.inject<CountriesContainer>())
      , countrySwitch(new osg::Switch())
      , countryHoverSwitch(new osg::Switch())
      , oSelectedCountryId(new osgGaming::Observable<int>(0))
      , highlightedBranchId(-1)
    {}

    void addCountry(
      int id,
      osg::Vec2f centerLatLong,
      osg::Vec2f size,
      osg::ref_ptr<osg::DrawElementsUInt> triangles,
      CountryNode::BorderIdMap& neighborBorders,
      osg::ref_ptr<osg::Vec3Array> vertices,
      osg::ref_ptr<osg::Vec2Array> texcoords1,
      osg::ref_ptr<osg::Vec3Array> texcoords2)
    {
      if (countryNodes.find(id) != countryNodes.end())
        return;

      CountryNode::Ptr node = new CountryNode(configManager, centerLatLong, size, vertices, texcoords1, texcoords2, triangles, neighborBorders);
      CountryHoverNode::Ptr hoverNode = new CountryHoverNode(vertices, texcoords1, triangles);

      countryNodes.insert(CountryNode::Map::value_type(id, node));
      countrySwitch->addChild(node, false);

      countryHoverNodes.insert(CountryHoverNode::Map::value_type(id, hoverNode));
      countryHoverSwitch->addChild(hoverNode, false);

      stateContainer->accessState([=](SimulationState::Ptr state)
      {
        CountryState::Ptr cstate = state->getCountryState(id);

        int n = skillsContainer->getNumBranches();
        for (int i = 0; i < n; i++)
        {
          std::string branchName = skillsContainer->getBranchByIndex(i)->getBranchName();

          skillBranchActivatedObservers.push_back(cstate->getOActivatedBranch(branchName.c_str())->connect(osgGaming::Func<bool>([=](bool activated)
          {
            Multithreading::uiExecuteOrAsync([=]()
            {
              if (!activated)
                return;

              if (oSelectedCountryId->get() == 0 && highlightedBranchId == i)
                setCountryColorMode(node, CountryNode::ColorMode(CountryNode::MODE_HIGHLIGHT_BANKS + i));
            });
          })));
        }
      });


    }

    void setCountryColorMode(CountryNode::Ptr mesh, CountryNode::ColorMode mode)
    {
      mesh->setColorMode(mode);
      countrySwitch->setChildValue(mesh, true);

      if (highlightedCountries.count(mesh) == 0)
        highlightedCountries.insert(mesh);
    }

    osg::ref_ptr<osgGaming::ResourceManager> resourceManager;
    osg::ref_ptr<osgGaming::TextureFactory> textureFactory;
    osg::ref_ptr<osgGaming::ShaderFactory> shaderFactory;
    osg::ref_ptr<ConfigManager> configManager;
    osg::ref_ptr<SimulationStateContainer> stateContainer;
    osg::ref_ptr<SkillsContainer> skillsContainer;
    CountriesContainer::Ptr countriesContainer;

    osg::ref_ptr<osg::Switch> countrySwitch;
    osg::ref_ptr<osg::Switch> countryHoverSwitch;

    CountryNode::Map countryNodes;
    CountryHoverNode::Map countryHoverNodes;
    CountriesMap::Ptr countriesMap;

    osgGaming::Observable<int>::Ptr oSelectedCountryId;

    int highlightedBranchId;

    std::vector<osgGaming::Observer<bool>::Ptr> skillBranchActivatedObservers;

    std::set<CountryNode::Ptr> highlightedCountries;
    CountryHoverNode::Ptr hoveredCountryNode;

    NeighbourMap neighbourMap;
  };

  CountryOverlay::CountryOverlay(osgGaming::Injector& injector)
    : osg::Group()
    , m(new Impl(injector, this))
  {
    addChild(m->countrySwitch);
    addChild(m->countryHoverSwitch);
  }

  CountryOverlay::~CountryOverlay()
  {
  }

  void CountryOverlay::loadCountries(
    std::string countriesFilename,
    std::string distanceMapFilename,
    osg::ref_ptr<osg::Vec3Array> vertices,
    osg::ref_ptr<osg::Vec2Array> texcoords)
  {
    // Load shaders
    osg::ref_ptr<osg::Program> cnProgram = new osg::Program();
    osg::ref_ptr<osg::Program> chnProgram = new osg::Program();

    osg::ref_ptr<osg::Shader> frag_shader = m->shaderFactory->make()
      ->type(osg::Shader::FRAGMENT)
      ->module(m->resourceManager->loadText(QString("./GameData/shaders/modules/%1.glsl").arg("noise3D").toStdString()))
      ->module(m->resourceManager->loadText("./GameData/shaders/country.frag"))
      ->build();

    osg::ref_ptr<osg::Shader> frag_shader_hover = m->shaderFactory->make()
      ->type(osg::Shader::FRAGMENT)
      ->module(m->resourceManager->loadText("./GameData/shaders/countryHover.frag"))
      ->build();

    osg::ref_ptr<osg::Shader> vert_shader = m->resourceManager->loadShader("./GameData/shaders/country.vert", osg::Shader::VERTEX);

    cnProgram->addShader(frag_shader);
    cnProgram->addShader(vert_shader);

    chnProgram->addShader(frag_shader_hover);
    chnProgram->addShader(vert_shader);

    // Load distance texture
    osg::ref_ptr<osg::Texture2D> distanceTexture = m->textureFactory->make()
      ->image(m->resourceManager->loadImage(distanceMapFilename))
      ->build();

    // Create statesets
    osg::ref_ptr<osg::StateSet> stateSet = getOrCreateStateSet();

    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    stateSet->setRenderBinDetails(1, "RenderBin");
    stateSet->addUniform(new osg::Uniform("distancemap", 0));
    stateSet->setTextureAttributeAndModes(0, distanceTexture, osg::StateAttribute::ON);

    osg::ref_ptr<osg::StateSet> csStateSet = m->countrySwitch->getOrCreateStateSet();
    csStateSet->setAttributeAndModes(cnProgram, osg::StateAttribute::ON);
    csStateSet->setRenderBinDetails(0, "RenderBin");

    osg::ref_ptr<osg::StateSet> chsStateSet = m->countryHoverSwitch->getOrCreateStateSet();
    chsStateSet->setAttributeAndModes(chnProgram, osg::StateAttribute::ON);
    chsStateSet->setRenderBinDetails(1, "RenderBin");

    m->neighbourMap.clear();

    // Calculate 2nd texcoord layer
    float radius = m->configManager->getNumber<float>("earth.radius");
    osg::ref_ptr<osg::Vec3Array> texcoords2 = new osg::Vec3Array();
    for (osg::Vec3Array::iterator it = vertices->begin(); it != vertices->end(); ++it)
      texcoords2->push_back((*it / radius + osg::Vec3f(1.0f, 1.0f, 1.0f)) / 2.0f);

    char* bytes = m->resourceManager->loadBinary(countriesFilename);

    osgGaming::ByteStream stream(bytes);

    int ncountries = stream.read<int>();
    for (int i = 0; i < ncountries; i++)
    {
      int id = stream.read<int>();
      float centerX = stream.read<float>();
      float centerY = stream.read<float>();
      float width = stream.read<float>();
      float height = stream.read<float>();

      osg::Vec2f centerLatLong((0.5f - centerY) * C_PI, fmodf(centerX + 0.5f, 1.0f) * 2.0f * C_PI);
      osg::Vec2f size(width, height);

      Country::Ptr country = m->countriesContainer->getCountry(id);

      NeighborList neighborList;

      int neighbors_count = stream.read<int>();
      for (int j = 0; j < neighbors_count; j++)
      {
        int neighbourId = stream.read<int>();
        neighborList.push_back(neighbourId);

        if (country.valid())
          country->getNeighbourIds().push_back(neighbourId);
      }

      m->neighbourMap.insert(NeighbourMap::value_type(id, neighborList));

      CountryNode::BorderIdMap neighborBorderMap;
      int neighborBorderCount = stream.read<int>();
      for (int j = 0; j < neighborBorderCount; j++)
      {
        std::vector<int> borders;

        int nid = stream.read<int>();
        int bcount = stream.read<int>();
        for (int k = 0; k < bcount; k++)
          borders.push_back(stream.read<int>());

        neighborBorderMap[nid] = borders;
      }

      osg::ref_ptr<osg::DrawElementsUInt> triangles = new osg::DrawElementsUInt(GL_TRIANGLES, 0);
      int triangles_count = stream.read<int>();
      for (int j = 0; j < triangles_count; j++)
      {
        int v0 = stream.read<int>();
        int v1 = stream.read<int>();
        int v2 = stream.read<int>();

        triangles->push_back(v0);
        triangles->push_back(v2);
        triangles->push_back(v1);
      }

      m->addCountry(id, centerLatLong, size, triangles, neighborBorderMap, vertices, texcoords, texcoords2);
    }

    for (CountryNode::Map::iterator it = m->countryNodes.begin(); it != m->countryNodes.end(); ++it)
    {
      NeighborList neighborList = m->neighbourMap.find(it->first)->second;

      for (NeighborList::iterator nit = neighborList.begin(); nit != neighborList.end(); ++nit)
      {
        it->second->addNeighbor(getCountryNode(*nit));
      }
    }

    int mapWidth = stream.read<int>();
    int mapHeight = stream.read<int>();

    m->countriesMap = new CountriesMap(mapWidth, mapHeight, reinterpret_cast<unsigned char*>(&bytes[stream.getPos()]));
  }

  void CountryOverlay::clearHighlightedCountries()
  {
    setAllCountriesVisibility(false);
    m->highlightedBranchId = -1;

    m->highlightedCountries.clear();
  }

  void CountryOverlay::setSelectedCountry(int countryId)
  {
    m->oSelectedCountryId->set(countryId);

    clearHighlightedCountries();

    if (countryId == 0)
      return;

    m->highlightedBranchId = -1;

    CountryNode::Ptr node = m->countryNodes.find(countryId)->second;
    m->setCountryColorMode(node, CountryNode::MODE_SELECTED);

    CountryNode::List& neighbors = node->getNeighborCountryNodes();
    for (CountryNode::List::iterator it = neighbors.begin(); it != neighbors.end(); ++it)
      m->setCountryColorMode(*it, CountryNode::MODE_NEIGHBOR);
  }

  void CountryOverlay::setHighlightedSkillBranch(int id)
  {
    m->oSelectedCountryId->set(0);
    clearHighlightedCountries();

    m->highlightedBranchId = id;

    std::string branchName = m->skillsContainer->getBranchByIndex(id)->getBranchName();

    for (CountryNode::Map::iterator it = m->countryNodes.begin(); it != m->countryNodes.end(); ++it)
    {
      int cid = it->first;
      m->stateContainer->accessState([=](SimulationState::Ptr state)
      {
        CountryState::Ptr cstate = state->getCountryState(cid);

        if (cstate->getBranchActivated(branchName.c_str()))
          m->setCountryColorMode(it->second, CountryNode::ColorMode(int(CountryNode::MODE_HIGHLIGHT_BANKS) + id));
      });
    }
  }

  void CountryOverlay::setHoveredCountryId(int id)
  {
    CountryHoverNode::Ptr countryHoverNode = m->countryHoverNodes.count(id) > 0 ? m->countryHoverNodes[id] : nullptr;

    if (countryHoverNode == m->hoveredCountryNode)
      return;

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

  void CountryOverlay::setAllCountriesVisibility(bool visibility)
  {
    if (visibility)
      m->countrySwitch->setAllChildrenOn();
    else
      m->countrySwitch->setAllChildrenOff();
  }

  CountryNode::Map& CountryOverlay::getCountryNodes()
  {
    return m->countryNodes;
  }

  CountriesMap::Ptr CountryOverlay::getCountriesMap()
  {
    return m->countriesMap;
  }

  CountryOverlay::NeighbourMap& CountryOverlay::getNeighbourships()
  {
    return m->neighbourMap;
  }

  CountryNode::Ptr CountryOverlay::getSelectedCountryNode()
  {
    return m->countryNodes.find(m->oSelectedCountryId->get())->second;
  }

  CountryNode::Ptr CountryOverlay::getCountryNode(int id)
  {
    CountryNode::Map::iterator countryNode = m->countryNodes.find(id);

    if (countryNode == m->countryNodes.end())
      return nullptr;

    return m->countryNodes.find(id)->second;
  }

  CountryNode::Ptr CountryOverlay::getCountryNode(osg::Vec2f coord)
  {
    int id = getCountryId(coord);

    if (id == 0)
      return nullptr;

    return getCountryNode(id);
  }

  int CountryOverlay::getCountryId(osg::Vec2f coord)
  {
    osg::Vec2i mapSize = m->countriesMap->getSize();

    int ix = int(coord.x() * float(mapSize.x()));
    int iy = int(coord.y() * float(mapSize.y()));

    return m->countriesMap->getDataAt(ix, iy);
  }

  int CountryOverlay::getSelectedCountryId()
  {
    return m->oSelectedCountryId->get();
  }

  osgGaming::Observable<int>::Ptr CountryOverlay::getSelectedCountryIdObservable()
  {
    return m->oSelectedCountryId;
  }
}