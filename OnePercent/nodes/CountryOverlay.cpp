#include "CountryOverlay.h"

#include "core/Macros.h"
#include "simulation/SimulatedValuesContainer.h"

#include <osgGaming/Observable.h>
#include <osgGaming/ByteStream.h>

#include <osgGaming/ResourceManager.h>
#include <osgGaming/TextureFactory.h>
#include <osgGaming/Helper.h>

#include <osg/Texture2D>

namespace onep
{
  struct CountryOverlay::Impl
  {
    Impl(osgGaming::Injector& injector, CountryOverlay* b)
      : base(b)
      , resourceManager(injector.inject<osgGaming::ResourceManager>())
      , textureFactory(injector.inject<osgGaming::TextureFactory>())
      , propertiesManager(injector.inject<osgGaming::PropertiesManager>())
      , skillsContainer(injector.inject<SkillsContainer>())
      , valuesContainer(injector.inject<SimulatedValuesContainer>())
      , oSelectedCountryId(new osgGaming::Observable<int>(0))
      , highlightedBranchId(-1)
    {}

    void addCountry(
      int id,
      CountryData::Ptr countryData,
      osg::ref_ptr<osg::DrawElementsUInt> triangles,
      CountryMesh::BorderIdMap& neighborBorders,
      osg::ref_ptr<osg::Vec3Array> vertices,
      osg::ref_ptr<osg::Vec2Array> texcoords)
    {
      if (countryMeshs.find(id) != countryMeshs.end())
        return;

      CountryMesh::Ptr mesh = new CountryMesh(countryData, vertices, texcoords, triangles, neighborBorders);

      countryMeshs.insert(CountryMesh::Map::value_type(id, mesh));
      base->addChild(mesh, false);

      assert(valuesContainer->getState()->getCountryStates().count(id) > 0);
      CountryState::Ptr cstate = valuesContainer->getState()->getCountryStates()[id];

      int n = skillsContainer->getNumBranches();
      for (int i = 0; i < n; i++)
      {
        std::string branchName = skillsContainer->getBranchByIndex(i)->getBranchName();

        skillBranchActivatedObservers.push_back(cstate->getOActivatedBranch(branchName.c_str())->connect(osgGaming::Func<bool>([this, mesh, i](bool activated)
        {
          if (!activated)
            return;

          if (oSelectedCountryId->get() == 0 && highlightedBranchId == i)
            setCountryColorMode(mesh, CountryMesh::ColorMode(CountryMesh::MODE_HIGHLIGHT_BANKS + i));
        })));
      }
    }

    void setCountryColorMode(CountryMesh::Ptr mesh, CountryMesh::ColorMode mode)
    {
      mesh->setColorMode(mode);
      base->setChildValue(mesh, true);

      if (highlightedCountries.count(mesh) == 0)
        highlightedCountries.insert(mesh);
    }

    CountryOverlay* base;

    osg::ref_ptr<osgGaming::ResourceManager> resourceManager;
    osg::ref_ptr<osgGaming::TextureFactory> textureFactory;
    osg::ref_ptr<osgGaming::PropertiesManager> propertiesManager;
    osg::ref_ptr<SkillsContainer> skillsContainer;
    SimulatedValuesContainer::Ptr valuesContainer;

    CountryMesh::Map countryMeshs;
    CountriesMap::Ptr countriesMap;

    osgGaming::Observable<int>::Ptr oSelectedCountryId;

    int highlightedBranchId;

    std::vector<osgGaming::Observer<bool>::Ptr> skillBranchActivatedObservers;

    std::set<CountryMesh::Ptr> highlightedCountries;
    CountryMesh::Ptr hoveredCountryMesh;

    NeighbourMap neighbourMap;
  };

  CountryOverlay::CountryOverlay(osgGaming::Injector& injector)
    : osg::Switch()
    , m(new Impl(injector, this))
  {
  }

  CountryOverlay::~CountryOverlay()
  {
  }

  void CountryOverlay::loadCountries(
    const std::map<int, std::string> idCountriesMap,
    std::string countriesFilename,
    std::string distanceMapFilename,
    osg::ref_ptr<osg::Vec3Array> vertices,
    osg::ref_ptr<osg::Vec2Array> texcoords)
  {
    osg::ref_ptr<osg::Program> program = new osg::Program();
    osg::ref_ptr<osg::Shader> frag_shader = m->resourceManager->loadShader("./GameData/shaders/country.frag", osg::Shader::FRAGMENT);
    osg::ref_ptr<osg::Shader> vert_shader = m->resourceManager->loadShader("./GameData/shaders/country.vert", osg::Shader::VERTEX);
    program->addShader(frag_shader);
    program->addShader(vert_shader);

    osg::ref_ptr<osg::StateSet> stateSet = getOrCreateStateSet();

    stateSet->setAttributeAndModes(program, osg::StateAttribute::ON);
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    stateSet->setRenderBinDetails(1, "RenderBin");

    m->neighbourMap.clear();

    char* bytes = m->resourceManager->loadBinary(countriesFilename);

    osgGaming::ByteStream stream(bytes);

    osg::ref_ptr<osg::Texture2D> distanceTexture = m->textureFactory->make()
      ->image(m->resourceManager->loadImage(distanceMapFilename))
      ->build();

    getOrCreateStateSet()->setTextureAttributeAndModes(0, distanceTexture, osg::StateAttribute::ON);
    getOrCreateStateSet()->addUniform(new osg::Uniform("distancemap", 0));

    int ncountries = stream.read<int>();
    for (int i = 0; i < ncountries; i++)
    {
      float population = stream.read<float>();
      float wealth = float(stream.read<int>());
      int id = stream.read<int>();
      float centerX = stream.read<float>();
      float centerY = stream.read<float>();
      float width = stream.read<float>();
      float height = stream.read<float>();

      std::string name;
      if (!idCountriesMap.count(id))
      {
        OSGG_QLOG_WARN(QString("No country name for id %1 defined").arg(id));
        name = "unknown";
        assert(false);
      }
      else
      {
        name = idCountriesMap.find(id)->second;
      }

      osg::ref_ptr<CountryData> country = new CountryData(
        m->propertiesManager,
        m->skillsContainer,
        name,
        id,
        population,
        wealth,
        osg::Vec2f((0.5f - centerY) * C_PI, fmodf(centerX + 0.5f, 1.0f) * 2.0f * C_PI),
        osg::Vec2f(width, height));

      NeighborList neighborList;

      int neighbors_count = stream.read<int>();
      for (int j = 0; j < neighbors_count; j++)
        neighborList.push_back(stream.read<int>());

      m->neighbourMap.insert(NeighbourMap::value_type(id, neighborList));

      CountryMesh::BorderIdMap neighborBorderMap;
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

      m->addCountry(int(id), country, triangles, neighborBorderMap, vertices, texcoords);
    }

    for (CountryMesh::Map::iterator it = m->countryMeshs.begin(); it != m->countryMeshs.end(); ++it)
    {
      NeighborList neighborList = m->neighbourMap.find(it->second->getCountryData()->getId())->second;

      for (NeighborList::iterator nit = neighborList.begin(); nit != neighborList.end(); ++nit)
      {
        it->second->addNeighbor(getCountryMesh(*nit), new NeighborCountryInfo());
      }
    }

    int mapWidth = stream.read<int>();
    int mapHeight = stream.read<int>();

    m->countriesMap = new CountriesMap(mapWidth, mapHeight, reinterpret_cast<unsigned char*>(&bytes[stream.getPos()]));

    m->resourceManager->clearCacheResource(countriesFilename);
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

    CountryMesh::Ptr mesh = m->countryMeshs.find(countryId)->second;
    m->setCountryColorMode(mesh, CountryMesh::MODE_SELECTED);

    CountryMesh::List& neighbors = mesh->getNeighborCountryMeshs();
    for (CountryMesh::List::iterator it = neighbors.begin(); it != neighbors.end(); ++it)
      m->setCountryColorMode(*it, CountryMesh::MODE_NEIGHBOR);
  }

  void CountryOverlay::setHighlightedSkillBranch(int id)
  {
    m->oSelectedCountryId->set(0);
    clearHighlightedCountries();

    m->highlightedBranchId = id;

    std::string branchName = m->skillsContainer->getBranchByIndex(id)->getBranchName();

    for (CountryMesh::Map::iterator it = m->countryMeshs.begin(); it != m->countryMeshs.end(); ++it)
    {
      int cid = it->second->getCountryData()->getId();
      CountryState::Ptr cstate = m->valuesContainer->getState()->getCountryStates()[cid];

      if (cstate->getOActivatedBranch(branchName.c_str())->get())
        m->setCountryColorMode(it->second, CountryMesh::ColorMode(int(CountryMesh::MODE_HIGHLIGHT_BANKS) + id));
    }
  }

  void CountryOverlay::setHoveredCountry(CountryMesh::Ptr countryMesh)
  {
    if (countryMesh == m->hoveredCountryMesh)
      return;

    if (m->hoveredCountryMesh)
    {
      m->hoveredCountryMesh->setHoverMode(false);

      if (m->highlightedCountries.count(m->hoveredCountryMesh) == 0)
        setChildValue(m->hoveredCountryMesh, false);
    }

    m->hoveredCountryMesh = countryMesh;

    if (!m->hoveredCountryMesh.valid())
      return;

    if (m->highlightedCountries.count(m->hoveredCountryMesh) == 0)
    {
      m->hoveredCountryMesh->setColorMode(CountryMesh::MODE_HOVER);
      setChildValue(m->hoveredCountryMesh, true);
    }

    m->hoveredCountryMesh->setHoverMode(true);
  }

  void CountryOverlay::setAllCountriesVisibility(bool visibility)
  {
    if (visibility)
      setAllChildrenOn();
    else
      setAllChildrenOff();
  }

  CountryMesh::Map& CountryOverlay::getCountryMeshs()
  {
    return m->countryMeshs;
  }

  CountriesMap::Ptr CountryOverlay::getCountriesMap()
  {
    return m->countriesMap;
  }

  CountryOverlay::NeighbourMap& CountryOverlay::getNeighbourships()
  {
    return m->neighbourMap;
  }

  CountryMesh::Ptr CountryOverlay::getSelectedCountryMesh()
  {
    return m->countryMeshs.find(m->oSelectedCountryId->get())->second;
  }

  CountryMesh::Ptr CountryOverlay::getCountryMesh(int id)
  {
    CountryMesh::Map::iterator countryMesh = m->countryMeshs.find(id);

    if (countryMesh == m->countryMeshs.end())
      return nullptr;

    return m->countryMeshs.find(id)->second;
  }

  CountryMesh::Ptr CountryOverlay::getCountryMesh(osg::Vec2f coord)
  {
    int id = getCountryId(coord);

    if (id == 0)
      return nullptr;

    return getCountryMesh(id);
  }

  int CountryOverlay::getCountryId(osg::Vec2f coord)
  {
    osg::Vec2i mapSize = m->countriesMap->getSize();

    int ix = int(coord.x() * float(mapSize.x()));
    int iy = int(coord.y() * float(mapSize.y()));

    return m->countriesMap->getDataAt(ix, iy);
  }

  std::string CountryOverlay::getCountryName(osg::Vec2f coord)
  {
    CountryMesh::Map::iterator it = m->countryMeshs.find(getCountryId(coord));
    if (it == m->countryMeshs.end())
    {
      return "No country selected";
    }

    return it->second->getCountryData()->getCountryName();
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