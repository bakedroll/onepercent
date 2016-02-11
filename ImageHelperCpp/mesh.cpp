#include "mesh.h"
#include <osgGaming/Helper.h>
#include "io.h"

namespace helper
{
  typedef struct _direction
  {
    _direction(int p1, int mid, int p2)
      : m_p1(p1)
      , m_mid(mid)
      , m_p2(p2)
    {
      
    }

    int m_p1;
    int m_mid;
    int m_p2;

    bool operator<(const _direction& other) const
    {
      if (m_p1 == other.m_p1)
      {
        if (m_mid == other.m_mid)
          return m_p2 < other.m_p2;

        return m_mid < other.m_mid;
      }

      return m_p1 < other.m_p1;
    }
  } Direction;

  typedef std::map<Direction, int> DirectionPointMap;

  void makeCartesianPoints(Graph& graph, IdPoint3DMap& points, float radius)
  {
    float width = graph.boundary.width();
    float height = graph.boundary.height();

    float pi2 = 2.0f * C_PI;
    for (IdPointMap::iterator it = graph.points.begin(); it != graph.points.end(); ++it)
    {
      float x = it->second.x / width;
      float y = it->second.y / height;

      osg::Vec3f vec = osgGaming::getCartesianFromPolar(osg::Vec2f(-y * C_PI + 0.5f * C_PI, x * pi2 - C_PI)) * radius;
      points.insert(IdPoint3DMap::value_type(it->first, vec));
    }
  }

  int getPoint(IdPoint3DMap& points, DirectionPointMap& directions, int p1id, int midid, int p2id, int& id, float thickness)
  {
    DirectionPointMap::iterator it = directions.find(Direction(p1id, midid, p2id));
    if (it != directions.end())
      return it->second;

    osg::Vec3f p1 = points.find(p1id)->second;
    osg::Vec3f mid = points.find(midid)->second;
    osg::Vec3f p2 = points.find(p2id)->second;

    osg::Vec3f normal = mid;
    normal.normalize();

    osg::Vec3 vp1 = (p1 - mid);
    osg::Vec3 vp2 = (p2 - mid);
    vp1.normalize();
    vp2.normalize();

    float a = (3.0f - (vp1 * vp2) * C_PI) / 4.0f;

    osg::Vec3f c1 = normal ^ vp1;
    osg::Vec3f c2 = normal ^ vp2;
    c1.normalize();
    c2.normalize();

    osg::Vec3f l = c1 - c2;
    l.normalize();
    l = l * (thickness / std::max<float>(sin(a), 0.5f));

    points.insert(IdPoint3DMap::value_type(++id, mid - l));
    directions.insert(DirectionPointMap::value_type(Direction(p1id, midid, p2id), id));

    return id;
  }

  bool getPointIds(NeighbourMap& neighbours, DirectionPointMap& directions, SphericalMesh& mesh, Graph& graph, int p1id, int midid, int& l, int & r, int& id, float thickness)
  {
    NeighbourMap::iterator it = neighbours.find(midid);
    assert(it->second.size() > 1);

    int p2lid = -1;
    int p2rid = -1;
    if (it->second.size() == 2)
    {
      for (NeighbourValueList::iterator nit = it->second.begin(); nit != it->second.end(); ++nit)
      {
        if (nit->first != p1id)
        {
          p2lid = nit->first;
          p2rid = nit->first;
          break;
        }
      }
    }
    else
    {
      AnglePointIdValueMap angles;
      makeAnglePointMap(graph, it->second, midid, p1id, angles);

      p2lid = angles.begin()->second.first;
      p2rid = angles.rbegin()->second.first;
    }

    if (p2lid > -1 && p2rid > -1)
    {
      l = getPoint(mesh.points, directions, p1id, midid, p2lid, id, thickness);
      r = getPoint(mesh.points, directions, p2rid, midid, p1id, id, thickness);
      return true;
    }

    return false;
  }

  void makeSphericalMesh(Graph& graph, SphericalMesh& mesh, float thickness)
  {
    ProgressPrinter progress("Make spherical mesh");

    DirectionPointMap directions;

    makeCartesianPoints(graph, mesh.points, 6.371f); // 1.001f
    mesh.edges = graph.edges;

    IdPoint3DMap removedPoints;
    removeUnusedPoints(mesh.points, mesh.edges, removedPoints);
    mesh.points = removedPoints;

    mesh.nInnerPoints = int(mesh.points.size());

    // get current id
    int id = -1;
    for (IdPoint3DMap::iterator it = mesh.points.begin(); it != mesh.points.end(); ++it)
      id = std::max(id, it->first);

    NeighbourMap neighbours;
    neighbourMapFromEdges(mesh.edges, neighbours);
    
    int i = 0;
    int max = mesh.edges.size();
    for (EdgeValueList::iterator it = mesh.edges.begin(); it != mesh.edges.end(); ++it)
    {
      int p1, p2, p3, p4;
      if (getPointIds(neighbours, directions, mesh, graph, it->first.second, it->first.first, p1, p2, id, thickness) &&
        getPointIds(neighbours, directions, mesh, graph, it->first.first, it->first.second, p3, p4, id, thickness))
      {
        mesh.quads.push_back(Quad(it->first.second, p3, p2, it->first.first));
        mesh.quads.push_back(Quad(it->first.first, p1, p4, it->first.second));
      }

      i++;
      progress.update(i, max);
    }
  }
}