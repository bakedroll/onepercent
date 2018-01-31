#include "CountryOverlay.h"

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
      , oSelectedCountryId(new osgGaming::Observable<int>(0))
      , highlightedBranch(BRANCH_UNDEFINED)
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

      for (int i = 0; i < NUM_SKILLBRANCHES; i++)
      {
        skillBranchActivatedObservers.push_back(countryData->getSkillBranchActivatedObservable(i)->connect(osgGaming::Func<bool>([this, mesh, i](bool activated)
        {
          if (!activated)
            return;

          if (oSelectedCountryId->get() == 0 && highlightedBranch == i)
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

    CountryMesh::Map countryMeshs;
    CountriesMap::Ptr countriesMap;

    osgGaming::Observable<int>::Ptr oSelectedCountryId;

    BranchType highlightedBranch;

    std::vector<osgGaming::Observer<bool>::Ptr> skillBranchActivatedObservers;

    std::set<CountryMesh::Ptr> highlightedCountries;
    CountryMesh::Ptr hoveredCountryMesh;
  };

  CountryOverlay::CountryOverlay(osgGaming::Injector& injector)
    : osg::Switch()
    , m(new Impl(injector, this))
  {
  }

  CountryOverlay::~CountryOverlay()
  {
  }

  void CountryOverlay::loadCountries(std::string countriesFilename, std::string distanceMapFilename, osg::ref_ptr<osg::Vec3Array> vertices, osg::ref_ptr<osg::Vec2Array> texcoords)
  {
    typedef std::vector<int> NeighborList;
    typedef std::map<int, NeighborList> NeighborMap;

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


    char* bytes = m->resourceManager->loadBinary(countriesFilename);

    osgGaming::ByteStream stream(bytes);

    NeighborMap neighborMap;

    osg::ref_ptr<osg::Texture2D> distanceTexture = m->textureFactory->make()
      ->image(m->resourceManager->loadImage(distanceMapFilename))
      ->build();

    getOrCreateStateSet()->setTextureAttributeAndModes(0, distanceTexture, osg::StateAttribute::ON);
    getOrCreateStateSet()->addUniform(new osg::Uniform("distancemap", 0));

    int ncountries = stream.read<int>();
    for (int i = 0; i < ncountries; i++)
    {
      int name_length = stream.read<int>();
      char* name_p = stream.readString(name_length);
      std::string name = name_p;

      float population = stream.read<float>();
      float wealth = float(stream.read<int>());
      int id = stream.read<int>();
      float centerX = stream.read<float>();
      float centerY = stream.read<float>();
      float width = stream.read<float>();
      float height = stream.read<float>();

      osg::ref_ptr<CountryData> country = new CountryData(
        m->propertiesManager,
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

      neighborMap.insert(NeighborMap::value_type(id, neighborList));

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

      delete[] name_p;
    }

    for (CountryMesh::Map::iterator it = m->countryMeshs.begin(); it != m->countryMeshs.end(); ++it)
    {
      NeighborList neighborList = neighborMap.find(it->second->getCountryData()->getId())->second;

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
    m->highlightedBranch = BRANCH_UNDEFINED;

    m->highlightedCountries.clear();
  }

  void CountryOverlay::setSelectedCountry(int countryId)
  {
    m->oSelectedCountryId->set(countryId);

    clearHighlightedCountries();

    if (countryId == 0)
      return;

    m->highlightedBranch = BRANCH_UNDEFINED;

    CountryMesh::Ptr mesh = m->countryMeshs.find(countryId)->second;
    m->setCountryColorMode(mesh, CountryMesh::MODE_SELECTED);

    CountryMesh::List& neighbors = mesh->getNeighborCountryMeshs();
    for (CountryMesh::List::iterator it = neighbors.begin(); it != neighbors.end(); ++it)
      m->setCountryColorMode(*it, CountryMesh::MODE_NEIGHBOR);
  }

  void CountryOverlay::setHighlightedSkillBranch(BranchType type)
  {
    m->oSelectedCountryId->set(0);
    clearHighlightedCountries();

    m->highlightedBranch = type;

    for (CountryMesh::Map::iterator it = m->countryMeshs.begin(); it != m->countryMeshs.end(); ++it)
    {
      if (it->second->getCountryData()->getSkillBranchActivated(type))
        m->setCountryColorMode(it->second, CountryMesh::ColorMode(int(CountryMesh::MODE_HIGHLIGHT_BANKS) + int(type)));
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