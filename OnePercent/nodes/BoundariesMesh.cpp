#include "BoundariesMesh.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/ByteStream.h>

#include <assert.h>

namespace onep
{
  struct Quad
  {
    typedef std::vector<Quad> List;
    int idx[4];
  };

  typedef std::map<int, Quad::List> IdQuadListMap;

  struct Point
  {
    typedef std::map<int, Point> Map;

    Point() {}
    Point(osg::Vec3f c, osg::Vec2f t, int o)
      : coords(c)
      , texcoord(t)
      , originId(o)
    {}

    osg::Vec3f coords;
    osg::Vec2f texcoord;
    int originId;
  };

  typedef std::vector<int> IdList;

  struct Border
  {
    int countryId;
    int neighborId;
    int boundarySegmentId;
    bool bIsCycle;
    IdList nextBorderIds;
  };

  typedef std::map<int, int> IdMap;
  typedef std::map<std::pair<int, int>, Quad::List> NodalsMap;
  typedef std::map<int, Border> BordersMap;

  struct BoundariesMesh::Impl
  {
    Impl(osgGaming::Injector& injector)
      : resourceManager(injector.inject<osgGaming::ResourceManager>())
    {}

    void addQuads(
      Quad::List& quads,
      float thickness,
      const osg::Vec3f& color,
      int& idCounter,
      IdMap& idMap,
      osg::ref_ptr<osg::Vec3Array> verts,
      osg::ref_ptr<osg::Vec4Array> colors,
      osg::ref_ptr<osg::DrawElementsUInt> unit)
    {
      for (Quad::List::iterator it = quads.begin(); it != quads.end(); ++it)
      {
        for (int i = 0; i < 4; i++)
        {
          int index = it->idx[i];

          IdMap::iterator iit = idMap.find(index);
          if (iit == idMap.end())
          {
            Point::Map::iterator pit = pointsMap.find(index);
            if (pit == pointsMap.end())
            {
              assert(false);
              return;
            }

            if (pit->second.originId > -1)
            {
              verts->push_back(pointsMap[pit->second.originId].coords + pit->second.coords * thickness);
              colors->push_back(osg::Vec4f(color, 0.0f));
            }
            else
            {
              verts->push_back(pit->second.coords);
              colors->push_back(osg::Vec4f(color, 1.0f));
            }

            int id = idCounter++;
            idMap[index] = id;
            unit->push_back(id);
          }
          else
          {
            unit->push_back(iit->second);
          }
        }
      }
    }

    osg::ref_ptr<osgGaming::ResourceManager> resourceManager;

    Point::Map pointsMap;
    IdQuadListMap boundariesMap;

    Quad::List nodalsFull;
    BordersMap borders;
    NodalsMap nodals;

    osg::ref_ptr<osg::Vec3Array> vertices;
    osg::ref_ptr<osg::Vec2Array> texcoords;

    osg::ref_ptr<osg::Geode> overallBoundsGeode;
    osg::ref_ptr<osg::Geode> countriesBoundsGeode;
  };

  BoundariesMesh::BoundariesMesh(osgGaming::Injector& injector)
    : osg::Group()
    , m(new Impl(injector))
  {
    osg::ref_ptr<osg::StateSet> stateSet = getOrCreateStateSet();

    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    stateSet->setRenderBinDetails(2, "RenderBin");
  }

  BoundariesMesh::~BoundariesMesh()
  {
  }

  void BoundariesMesh::loadBoundaries(const std::string& filename)
  {
    m->pointsMap.clear();
    m->boundariesMap.clear();
    m->borders.clear();
    m->nodals.clear();
    m->nodalsFull.clear();
    m->vertices.release();

    char* bytes = m->resourceManager->loadBinary(filename);
    osgGaming::ByteStream stream(bytes);

    int nverts = stream.read<int>();
    for (int i = 0; i < nverts; i++)
    {
      float x = stream.read<float>();
      float y = stream.read<float>();
      float z = stream.read<float>();
      int originId = stream.read<int>();

      float u = stream.read<float>();
      float v = stream.read<float>();

      m->pointsMap[i] = Point(osg::Vec3f(x, y, z), osg::Vec2f(u, 1.0f - v), originId);
    }

    int nsegments = stream.read<int>();
    for (int i = 0; i < nsegments; i++)
    {
      Quad::List segmentQuads;

      int id = stream.read<int>();
      int nquads = stream.read<int>();
      for (int j = 0; j < nquads; j++)
      {
        Quad quad;
        for (int k = 0; k < 4; k++)
          quad.idx[k] = stream.read<int>();

        segmentQuads.push_back(quad);
      }

      m->boundariesMap[id] = segmentQuads;
    }

    int nfullnodals = stream.read<int>();
    for (int i = 0; i < nfullnodals; i++)
    {
      int nquads = stream.read<int>();
      for (int j = 0; j < nquads; j++)
      {
        Quad quad;
        for (int k = 0; k < 4; k++)
          quad.idx[k] = stream.read<int>();

        m->nodalsFull.push_back(quad);
      }
    }

    int nborders = stream.read<int>();
    for (int i = 0; i < nborders; i++)
    {
      Border border;

      int id = stream.read<int>();
      border.countryId = stream.read<int>();
      border.neighborId = stream.read<int>();
      border.boundarySegmentId = stream.read<int>();
      border.bIsCycle = stream.read<bool>();

      int nNextBorders = stream.read<int>();
      for (int j = 0; j < nNextBorders; j++)
        border.nextBorderIds.push_back(stream.read<int>());

      m->borders[id] = border;
    }

    int nnodals = stream.read<int>();
    for (int i = 0; i < nnodals; i++)
    {
      std::pair<int, int> fromTo;
      fromTo.first = stream.read<int>();
      fromTo.second = stream.read<int>();

      Quad::List quads;

      int nquads = stream.read<int>();
      for (int j = 0; j < nquads; j++)
      {
        Quad quad;
        for (int k = 0; k < 4; k++)
          quad.idx[k] = stream.read<int>();

        quads.push_back(quad);
      }

      m->nodals[fromTo] = quads;
    }

    m->resourceManager->clearCacheResource(filename);
  }

  osg::ref_ptr<osg::Vec3Array> BoundariesMesh::getCountryVertices()
  {
    if (!m->vertices.valid())
    {
      m->vertices = new osg::Vec3Array();

      for (Point::Map::iterator it = m->pointsMap.begin(); it != m->pointsMap.end(); ++it)
      {
        if (it->second.originId == -1)
          m->vertices->push_back(it->second.coords);
        else
          break;
      }
    }

    return m->vertices;
  }

  osg::ref_ptr<osg::Vec2Array> BoundariesMesh::getCountryTexcoords()
  {
    if (!m->texcoords.valid())
    {
      m->texcoords = new osg::Vec2Array();

      for (Point::Map::iterator it = m->pointsMap.begin(); it != m->pointsMap.end(); ++it)
      {
        if (it->second.originId == -1)
          m->texcoords->push_back(it->second.texcoord);
        else
          break;
      }
    }

    return m->texcoords;
  }

  void BoundariesMesh::makeOverallBoundaries(float thickness)
  {
    if (m->overallBoundsGeode.valid())
      return;

    osg::Vec3f color(1.0f, 1.0f, 1.0f);

    m->overallBoundsGeode = new osg::Geode();

    osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();

    osg::ref_ptr<osg::DrawElementsUInt> quads = new osg::DrawElementsUInt(GL_QUADS, 0);


    int idCounter = 0;
    IdMap idMap;
    for (IdQuadListMap::iterator it = m->boundariesMap.begin(); it != m->boundariesMap.end(); ++it)
      m->addQuads(it->second, thickness, color, idCounter, idMap, verts, colors, quads);

    m->addQuads(m->nodalsFull, thickness, color, idCounter, idMap, verts, colors, quads);


    osg::ref_ptr<osg::Geometry> geo_quads = new osg::Geometry();
    geo_quads->setVertexArray(verts);
    geo_quads->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
    geo_quads->addPrimitiveSet(quads);

    m->overallBoundsGeode->addDrawable(geo_quads);

    addChild(m->overallBoundsGeode);
  }

  void BoundariesMesh::makeCountryBoundaries(const CountryMesh::List& countries, const osg::Vec3f& color, float thickness)
  {
    if (m->countriesBoundsGeode.valid())
    {
      removeChild(m->countriesBoundsGeode);
      m->countriesBoundsGeode.release();
    }

    if (countries.empty())
      return;

    m->countriesBoundsGeode = new osg::Geode();

    osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();

    osg::ref_ptr<osg::DrawElementsUInt> quads = new osg::DrawElementsUInt(GL_QUADS, 0);

    int idCounter = 0;
    IdMap idMap;

    std::map<int, CountryMesh::Ptr> cmap;
    for (CountryMesh::List::const_iterator it = countries.begin(); it != countries.end(); ++it)
      cmap[(*it)->getCountryData()->getId()] = *it;

    std::set<int> visited;

    for (CountryMesh::List::const_iterator it = countries.begin(); it != countries.end(); ++it)
    {
      CountryMesh::Ptr cmesh = *it;

      std::vector<int> startIds;

      if (cmesh->getNeighborBorders().count(-1) > 0)
      {
        const std::vector<int>& neighbors = cmesh->getNeighborBorderIds(-1);
        for (std::vector<int>::const_iterator nit = neighbors.begin(); nit != neighbors.end(); ++nit)
        {
          if (visited.count(*nit) == 0)
            startIds.push_back(*nit);
        }
      }
      else
      {
        const CountryMesh::BorderIdMap& nborders = cmesh->getNeighborBorders();
        for (CountryMesh::BorderIdMap::const_iterator nit = nborders.begin(); nit != nborders.end(); ++nit)
        {
          // neighbor not included, start at this border
          if (cmap.count(nit->first) == 0)
          {
            bool found = false;
            for (std::vector<int>::const_iterator nnit = nit->second.begin(); nnit != nit->second.end(); ++nnit)
            {
              if (visited.count(*nnit) == 0)
              {
                startIds.push_back(*nnit);
                found = true;
                break;
              }
            }

            if (found)
              break;
          }
        }
      }

      for (IdList::iterator iit = startIds.begin(); iit != startIds.end(); ++iit)
      {
        int startBorderId = *iit;
        if (startBorderId == -1 || visited.count(startBorderId) > 0)
          continue;

        int currentBorderId = startBorderId;
        do
        {
          Border& border = m->borders[currentBorderId];

          m->addQuads(m->boundariesMap[border.boundarySegmentId], thickness, color, idCounter, idMap, verts, colors, quads);
          visited.insert(currentBorderId);

          if (border.bIsCycle)
            break;

          for (IdList::iterator bit = border.nextBorderIds.begin(); bit != border.nextBorderIds.end(); ++bit)
          {
            Border& nextBorder = m->borders[*bit];
            if (border.countryId == nextBorder.countryId || cmap.count(nextBorder.countryId) > 0)
            {
              m->addQuads(m->nodals[std::pair<int, int>(currentBorderId, *bit)], thickness, color, idCounter, idMap, verts, colors, quads);
              currentBorderId = *bit;
              break;
            }
          }

        } while (currentBorderId != startBorderId);
      }
    }

    osg::ref_ptr<osg::Geometry> geo_quads = new osg::Geometry();
    geo_quads->setVertexArray(verts);
    geo_quads->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
    geo_quads->addPrimitiveSet(quads);

    m->countriesBoundsGeode->addDrawable(geo_quads);

    addChild(m->countriesBoundsGeode);
  }
}