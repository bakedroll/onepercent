#include "reduce.h"

namespace helper
{

  typedef std::pair<int, uchar> NeighbourValue;
  typedef std::vector<NeighbourValue> NeighbourValueList;
  typedef std::map<int, NeighbourValueList> NeighbourMap;

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

  void reducePoints(Graph& graph)
  {
    NeighbourMap neighbourMap;

    for (EdgeValueList::iterator eit = graph.edges.begin(); eit != graph.edges.end(); ++eit)
    {
      insertNeighbour(neighbourMap, eit->first.first, eit->first.second, eit->second);
      insertNeighbour(neighbourMap, eit->first.second, eit->first.first, eit->second);
    }

    return;
  }
}