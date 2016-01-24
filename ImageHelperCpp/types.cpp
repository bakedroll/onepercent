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

  void neighbourMapFromGraph(Graph& graph, NeighbourMap& neighbourMap)
  {
    for (EdgeValueList::iterator eit = graph.edges.begin(); eit != graph.edges.end(); ++eit)
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
}