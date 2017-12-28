#include "vectorizer/types.h"

#include <osgGaming/Helper.h>

namespace helper
{
  void insertNeighbour(NeighbourMap& map, int p1, int p2, uchar attr)
  {
    NeighbourMap::iterator it = map.find(p1);

    NeighbourValue value;
    value.first = p2;
    value.second = attr;

    if (it == map.end())
    {
      NeighbourValueList list;
      list.push_back(value);
      map.insert(NeighbourMap::value_type(p1, list));
    }
    else
    {
      it->second.push_back(value);
    }
  }

  void neighbourMapFromEdges(EdgeValueList& edges, NeighbourMap& neighbourMap)
  {
    for (EdgeValueList::iterator eit = edges.begin(); eit != edges.end(); ++eit)
    {
      insertNeighbour(neighbourMap, eit->first.first, eit->first.second, eit->second);
      insertNeighbour(neighbourMap, eit->first.second, eit->first.first, eit->second);
    }
  }

  void neighbourMapFromEdges(NeighbourEdgesMap& edges, NeighbourMap& neighbourMap)
  {
    for (NeighbourEdgesMap::iterator it = edges.begin(); it != edges.end(); ++it)
    {
      for (EdgeList::iterator eit = it->second.begin(); eit != it->second.end(); ++eit)
      {
        insertNeighbour(neighbourMap, eit->first, eit->second, it->first > -1 ? 255 : 128);
        insertNeighbour(neighbourMap, eit->second, eit->first, eit->second);
      }
    }
  }

  void neighbourMapFromEdges(EdgeList& edges, NeighbourMap& neighbourMap)
  {
    for (EdgeList::iterator it = edges.begin(); it != edges.end(); ++it)
    {
      insertNeighbour(neighbourMap, it->first, it->second, 0);
      insertNeighbour(neighbourMap, it->second, it->first, 0);
    }
  }

  void removeNeighbourFromList(NeighbourValueList& list, int id)
  {
    for (NeighbourValueList::iterator it = list.begin(); it != list.end(); ++it)
    {
      if (it->first == id)
      {
        list.erase(it);
        return;
      }
    }
  }

  bool removeNeighbourMapPoint(NeighbourMap& neighbourMap, int pointId, int& endpoint1, int& endpoint2, uchar& value)
  {
    NeighbourMap::iterator it = neighbourMap.find(pointId);

    assert(it->second.size() == 2);
    assert(it->second[0].second == it->second[1].second);
    
    value = it->second[0].second;
    endpoint1 = it->second[0].first;
    endpoint2 = it->second[1].first;

    NeighbourMap::iterator e1it = neighbourMap.find(endpoint1);
    NeighbourMap::iterator e2it = neighbourMap.find(endpoint2);

    for (NeighbourValueList::iterator nit = e1it->second.begin(); nit != e1it->second.end(); ++nit)
      if (nit->first == endpoint2)
        return false;

    neighbourMap.erase(it);

    removeNeighbourFromList(e1it->second, pointId);
    removeNeighbourFromList(e2it->second, pointId);

    return true;
  }

  double angleBetween(cv::Vec2f v1, cv::Vec2f v2)
  {
    double dot = v1.val[0] * v2.val[0] + v1.val[1] * v2.val[1];
    double det = v1.val[0] * v2.val[1] - v1.val[1] * v2.val[0];

    double angle = atan2(det, dot);
    if (angle < 0)
      angle += 2 * CV_PI;

    return angle;
  }

  void makeAnglePointMap(IdPointMap& points, NeighbourValueList& neighbours, int originId, int p1Id, AnglePointIdValueMap& angles)
  {
    cv::Vec2f p1;
    if (p1Id < 0)
      p1 = cv::Vec2f(-1.0f, 0.0f);
    else
      p1 = points[p1Id] - points[originId];

    for (NeighbourValueList::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
      if (it->first != p1Id)
        angles.insert(AnglePointIdValueMap::value_type(angleBetween(p1, points[it->first] - points[originId]), PointIdValue(it->first, it->second)));
  }

  void makePointTriangleMap(Graph& graph, PointTriangleMap& map)
  {
    for (TriangleMap::iterator it = graph.triangles.begin(); it != graph.triangles.end(); ++it)
    {
      for (int i = 0; i < 3; i++)
      {
        PointTriangleMap::iterator pit = map.find(it->second.idx[i]);
        if (pit == map.end())
        {
          IdSet set;
          set.insert(it->first);
          map.insert(PointTriangleMap::value_type(it->second.idx[i], set));

          continue;
        }

        pit->second.insert(it->first);
      }
    }
  }

  void makePointList(IdPointMap& points, IdSet& ids, PointListf& result)
  {
    for (IdSet::iterator it = ids.begin(); it != ids.end(); ++it)
      result.push_back(points.find(*it)->second);
  }

  void removeUnusedPoints(IdPoint3DMap& points, EdgeValueList& edges, IdPoint3DMap& results)
  {
    IdSet ids;
    for (EdgeValueList::iterator it = edges.begin(); it != edges.end(); ++it)
    {
      if (ids.find(it->first.first) == ids.end())
        ids.insert(it->first.first);

      if (ids.find(it->first.second) == ids.end())
        ids.insert(it->first.second);
    }

    for (IdSet::iterator it = ids.begin(); it != ids.end(); ++it)
      results.insert(IdPoint3DMap::value_type(*it, points.find(*it)->second));
  }

  void trace(NeighbourMap& neighbours, int p1id, int p2id, IdSet& visited, EdgeList& segment)
  {
    visited.insert(p2id);
    segment.push_back(Edge(p1id, p2id));

    NeighbourValueList& nlist = neighbours.find(p2id)->second;

    for (NeighbourValueList::iterator it = nlist.begin(); it != nlist.end(); ++it)
    {
      int pnext = it->first;
      if (pnext != p1id)
      {
        if (visited.count(pnext) == 0)
          trace(neighbours, p2id, pnext, visited, segment);
      }
    }
  }

  void findSegments(EdgeList& edges, EdgeListList& results)
  {
    NeighbourMap neighbours;
    neighbourMapFromEdges(edges, neighbours);

    IdSet visited;

    for (NeighbourMap::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
    {
      int p1id = it->first;

      if (visited.count(p1id) > 0)
        continue;

      visited.insert(p1id);
      EdgeList segment;
      for (NeighbourValueList::iterator nit = it->second.begin(); nit != it->second.end(); ++nit)
      {
        int p2id = nit->first;
        trace(neighbours, p1id, p2id, visited, segment);
      }

      results.push_back(segment);
    }
  }

  void makeCartesianPoints(Graph& graph, IdPoint3DMap& points, float radius, float shift)
  {
    float width = graph.boundary.width();
    float height = graph.boundary.height();

    float pi2 = 2.0f * C_PI;
    for (IdPointMap::iterator it = graph.points.begin(); it != graph.points.end(); ++it)
    {
      float x = (it->second.x + shift) / width;
      float y = it->second.y / height;

      osg::Vec3f vec = osgGaming::getCartesianFromPolar(osg::Vec2f((0.5f - y) * C_PI, x * pi2 - C_PI)) * radius;
      points.insert(IdPoint3DMap::value_type(it->first, Point3D(vec.x(), vec.y(), vec.z())));
    }
  }

  void makePolarPoints(IdPoint3DMap& input, IdPoint3DMap& output, int width, int height, float shift)
  {
    for (IdPoint3DMap::iterator it = input.begin(); it != input.end(); ++it)
    {
      int id = it->first;
      const Point3D& p3D = it->second;
      osg::Vec3f p(p3D.value[0], p3D.value[1], p3D.value[2]);

      if (p3D.originId > -1)
      {
        IdPoint3DMap::iterator pit = input.find(p3D.originId);
        osg::Vec3f porigin(pit->second.value[0], pit->second.value[1], pit->second.value[2]);

        p = porigin + (p * 0.1f);
      }

      osg::Vec2f result = osgGaming::getPolarFromCartesian(p);

      float x = result.x() * width - shift;
      float y = result.y() * height;

      output.insert(IdPoint3DMap::value_type(id, Point3D(x, y, 0.0f)));
    }
  }
}