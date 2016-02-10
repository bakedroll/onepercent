#include "mesh.h"
#include <osgGaming/Helper.h>

namespace helper
{
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

  int getPoint(IdPoint3DMap& points, int p1id, int midid, int p2id, int& id, float thickness)
  {
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
    l = l * (thickness / sin(a));

    points.insert(IdPoint3DMap::value_type(++id, mid + l));
    points.insert(IdPoint3DMap::value_type(++id, mid - l));

    return -1;
  }

  void makeSphericalMesh(Graph& graph, SphericalMesh& mesh, float thickness)
  {
    printf("Make spherical mesh");

    makeCartesianPoints(graph, mesh.points, 6.371f * 1.1f); // 1.001f
    mesh.edges = graph.edges;

    // get current id
    int id = -1;
    for (IdPoint3DMap::iterator it = mesh.points.begin(); it != mesh.points.end(); ++it)
      id = std::max(id, it->first);

    NeighbourMap neighbours;
    neighbourMapFromEdges(mesh.edges, neighbours);

    for (NeighbourMap::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
    {
      if (it->second.size() == 2)
      {
        getPoint(mesh.points, it->second[0].first, it->first, it->second[1].first, id, thickness);
      }
    }
  }
}