#include "data/BoundariesData.h"

#include <osgGaming/ByteStream.h>
#include <osgGaming/ResourceManager.h>

namespace onep
{

struct Edge
{
  int p1;
  int p2;
};

struct Quad
{
  using List = std::vector<Quad>;
  int idx[4] = {-1, -1, -1, -1};
};

using IdQuadListMap = std::map<int, Quad::List>;

struct Point
{
  using Map = std::map<int, Point>;

  Point() : originId(-1)
  {
  }
  Point(const osg::Vec3f& c, const osg::Vec2f& t, int o) : coords(c), texcoord(t), originId(o)
  {
  }

  osg::Vec3f coords;
  osg::Vec2f texcoord;
  int        originId;
};

using IdList = std::vector<int>;

struct Border
{
  int    countryId         = -1;
  int    neighborId        = -1;
  int    boundarySegmentId = -1;
  bool   bIsCycle          = false;
  IdList nextBorderIds;
};

struct QuadSegment
{
  using List = std::vector<QuadSegment>;

  Edge e1;
  Edge e2;
};

using IdMap      = std::map<int, int>;
using NodalsMap  = std::map<std::pair<int, int>, Quad::List>;
using BordersMap = std::map<int, Border>;

struct QuadSegments
{
  QuadSegments() : verts(new osg::Vec3Array())
  {
  }

  QuadSegment::List            segments;
  IdMap                        ids;
  osg::ref_ptr<osg::Vec3Array> verts;
};

struct BoundariesData::Impl
{
  Impl(osgGaming::Injector& injector) : resourceManager(injector.inject<osgGaming::ResourceManager>())
  {
  }

  void addQuads(Quad::List& quads, float thickness, QuadSegments& segments, const osg::Vec3f& color, int& idCounter,
                IdMap& idMap, osg::Vec3Array* verts, osg::Vec4Array* colors, osg::DrawElementsUInt* unit,
                bool bWireframe = false)
  {
    auto lastId = -1, currentId = -1, firstId = -1;

    for (const auto& quad : quads)
    {
      for (auto i = 0; i < 4; i++)
      {
        auto index = quad.idx[i];

        auto iit = idMap.find(index);
        if (iit == idMap.end())
        {
          auto pit = pointsMap.find(index);
          if (pit == pointsMap.end())
          {
            assert(false);
            return;
          }

          if (pit->second.originId > -1)
          {
            verts->push_back(pointsMap[pit->second.originId].coords + pit->second.coords * thickness);
            colors->push_back(osg::Vec4f(color, 0.2f));  // 0.0f
          }
          else
          {
            verts->push_back(pit->second.coords);
            colors->push_back(osg::Vec4f(color, 1.0f));
          }

          const auto id = idCounter++;
          idMap[index]  = id;
          if (bWireframe)
            currentId = id;
          else
            unit->push_back(id);

          if (bWireframe && i == 0)
            firstId = id;
        }
        else
        {
          if (bWireframe)
            currentId = iit->second;
          else
            unit->push_back(iit->second);

          if (bWireframe && i == 0)
            firstId = iit->second;
        }

        if (bWireframe)
        {
          if (i > 0 && i < 3)
          {
            unit->push_back(lastId);
            unit->push_back(currentId);
          }
          else if (i == 3)
          {
            unit->push_back(currentId);
            unit->push_back(firstId);
          }

          lastId = currentId;
        }
      }
    }
  }

  osg::ref_ptr<osgGaming::ResourceManager> resourceManager;

  Point::Map    pointsMap;
  IdQuadListMap boundariesMap;

  Quad::List nodalsFull;
  BordersMap borders;
  NodalsMap  nodals;

  osg::ref_ptr<osg::Vec3Array> vertices;
  osg::ref_ptr<osg::Vec2Array> texcoords;
};

BoundariesData::BoundariesData(osgGaming::Injector& injector) : osg::Referenced(), m(new Impl(injector))
{
}

BoundariesData::~BoundariesData() = default;

void BoundariesData::loadBoundaries(const std::string& filename)
{
  m->pointsMap.clear();
  m->boundariesMap.clear();
  m->borders.clear();
  m->nodals.clear();
  m->nodalsFull.clear();
  m->vertices.release();

  const auto            bytes = m->resourceManager->loadBinary(filename);
  osgGaming::ByteStream stream(bytes);

  const auto nverts = stream.read<int>();
  for (auto i = 0; i < nverts; i++)
  {
    const auto x        = stream.read<float>();
    const auto y        = stream.read<float>();
    const auto z        = stream.read<float>();
    const auto originId = stream.read<int>();

    const auto u = stream.read<float>();
    const auto v = stream.read<float>();

    m->pointsMap[i] = Point(osg::Vec3f(x, y, z), osg::Vec2f(u, 1.0f - v), originId);
  }

  const auto nsegments = stream.read<int>();
  for (auto i = 0; i < nsegments; i++)
  {
    Quad::List segmentQuads;

    const auto id     = stream.read<int>();
    const auto nquads = stream.read<int>();
    for (auto j = 0; j < nquads; j++)
    {
      Quad quad;
      for (int& k : quad.idx)
        k = stream.read<int>();

      segmentQuads.push_back(quad);
    }

    m->boundariesMap[id] = segmentQuads;
  }

  const auto nfullnodals = stream.read<int>();
  for (auto i = 0; i < nfullnodals; i++)
  {
    const auto nquads = stream.read<int>();
    for (auto j = 0; j < nquads; j++)
    {
      Quad quad;
      for (int& k : quad.idx)
        k = stream.read<int>();

      m->nodalsFull.push_back(quad);
    }
  }

  const auto nborders = stream.read<int>();
  for (auto i = 0; i < nborders; i++)
  {
    Border border;

    const auto id            = stream.read<int>();
    border.countryId         = stream.read<int>();
    border.neighborId        = stream.read<int>();
    border.boundarySegmentId = stream.read<int>();
    border.bIsCycle          = stream.read<bool>();

    const auto nNextBorders = stream.read<int>();
    for (auto j = 0; j < nNextBorders; j++)
      border.nextBorderIds.push_back(stream.read<int>());

    m->borders[id] = border;
  }

  const auto nnodals = stream.read<int>();
  for (auto i = 0; i < nnodals; i++)
  {
    std::pair<int, int> fromTo;
    fromTo.first  = stream.read<int>();
    fromTo.second = stream.read<int>();

    Quad::List quads;

    const auto nquads = stream.read<int>();
    for (auto j = 0; j < nquads; j++)
    {
      Quad quad;
      for (int& k : quad.idx)
        k = stream.read<int>();

      quads.push_back(quad);
    }

    m->nodals[fromTo] = quads;
  }
}

osg::ref_ptr<osg::Vec3Array> BoundariesData::getCountryVertices() const
{
  if (!m->vertices.valid())
  {
    m->vertices = new osg::Vec3Array();

    for (const auto& point : m->pointsMap)
    {
      if (point.second.originId == -1)
        m->vertices->push_back(point.second.coords);
      else
        break;
    }
  }

  return m->vertices;
}

osg::ref_ptr<osg::Vec2Array> BoundariesData::getCountryTexcoords() const
{
  if (!m->texcoords.valid())
  {
    m->texcoords = new osg::Vec2Array();

    for (const auto& point : m->pointsMap)
    {
      if (point.second.originId == -1)
        m->texcoords->push_back(point.second.texcoord);
      else
        break;
    }
  }

  return m->texcoords;
}

osg::ref_ptr<osg::Geometry> BoundariesData::createOverallBoundariesGeometry(float thickness) const
{
  const osg::Vec3f color(1.0f, 1.0f, 1.0f);

  QuadSegments segments;

  const auto verts  = new osg::Vec3Array();
  const auto colors = new osg::Vec4Array();
  const auto quads  = new osg::DrawElementsUInt(GL_QUADS, 0);

  auto  idCounter = 0;
  IdMap idMap;
  for (auto& bound : m->boundariesMap)
  {
    m->addQuads(bound.second, thickness, segments, color, idCounter, idMap, verts, colors, quads);
  }

  m->addQuads(m->nodalsFull, thickness, segments, color, idCounter, idMap, verts, colors, quads);

  const auto geo_quads = new osg::Geometry();
  geo_quads->setVertexArray(verts);
  geo_quads->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
  geo_quads->addPrimitiveSet(quads);

  return geo_quads;
}

osg::ref_ptr<osg::Geometry> BoundariesData::createCountryBoundariesGeometry(const CountryNode::Map& countries,
                                                                            const osg::Vec3f& color, float thickness,
                                                                            bool bWireframe) const
{
  auto geo_quads = new osg::Geometry();

  if (countries.empty())
    return geo_quads;

  const auto verts  = new osg::Vec3Array();
  const auto colors = new osg::Vec4Array();

  const auto unit = new osg::DrawElementsUInt(bWireframe ? GL_LINES : GL_QUADS, 0);

  auto  idCounter = 0;
  IdMap idMap;

  QuadSegments segments;

  std::set<int> visited;

  for (const auto& country : countries)
  {
    auto& cmesh = country.second;

    std::vector<int> startIds;

    if (cmesh->getNeighborBorders().count(-1) > 0)
    {
      const auto& neighbors = cmesh->getNeighborBorderIds(-1);
      for (const auto& neighbor : neighbors)
      {
        if (visited.count(neighbor) == 0)
          startIds.push_back(neighbor);
      }
    }
    else
    {
      const auto& nborders = cmesh->getNeighborBorders();
      for (const auto& nborder : nborders)
      {
        // neighbor not included, start at this border
        if (countries.count(nborder.first) == 0)
        {
          auto found = false;
          for (const auto& id : nborder.second)
          {
            if (visited.count(id) == 0)
            {
              startIds.push_back(id);
              found = true;
              break;
            }
          }

          if (found)
            break;
        }
      }
    }

    for (const auto& id : startIds)
    {
      auto startBorderId = id;
      if (startBorderId == -1 || visited.count(startBorderId) > 0)
        continue;

      auto currentBorderId = startBorderId;
      do
      {
        auto& border = m->borders[currentBorderId];

        m->addQuads(m->boundariesMap[border.boundarySegmentId], thickness, segments, color, idCounter, idMap, verts,
                    colors, unit, bWireframe);
        visited.insert(currentBorderId);

        if (border.bIsCycle)
          break;

        for (const auto& nborderId : border.nextBorderIds)
        {
          auto& nextBorder = m->borders[nborderId];
          if (border.countryId == nextBorder.countryId || countries.count(nextBorder.countryId) > 0)
          {
            m->addQuads(m->nodals[std::pair<int, int>(currentBorderId, nborderId)], thickness, segments, color,
                        idCounter, idMap, verts, colors, unit, bWireframe);
            currentBorderId = nborderId;
            break;
          }
        }

      } while (currentBorderId != startBorderId);
    }
  }

  geo_quads->setVertexArray(verts);
  geo_quads->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
  geo_quads->addPrimitiveSet(unit);

  return geo_quads;
}

}  // namespace onep
