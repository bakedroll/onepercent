#include "types.h"

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

  void removeNeighbourMapPoint(NeighbourMap& neighbourMap, int pointId, int& endpoint1, int& endpoint2, uchar& value)
  {
    NeighbourMap::iterator it = neighbourMap.find(pointId);

    assert(it->second.size() == 2);
    assert(it->second[0].second == it->second[1].second);

    value = it->second[0].second;
    endpoint1 = it->second[0].first;
    endpoint2 = it->second[1].first;

    neighbourMap.erase(it);

    removeNeighbourFromList(neighbourMap.find(endpoint1)->second, pointId);
    removeNeighbourFromList(neighbourMap.find(endpoint2)->second, pointId);
  }

  void makeFloatFloatIdMap(Graph& graph, FloatFloatIdMap& map)
  {
    for (IdPointMap::iterator it = graph.points.begin(); it != graph.points.end(); ++it)
    {
      FloatFloatIdMap::iterator itx = map.find(it->second.x);
      if (itx != map.end())
      {
        FloatIdMap::iterator ity = itx->second.find(it->second.y);
        if (ity != itx->second.end())
          continue;

        itx->second.insert(FloatIdMap::value_type(it->second.y, it->first));
      }
      else
      {
        FloatIdMap fmap;
        fmap.insert(FloatIdMap::value_type(it->second.y, it->first));
        map.insert(FloatFloatIdMap::value_type(it->second.x, fmap));
      }
    }
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
}