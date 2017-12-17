#include "check.h"
#include <set>

namespace helper
{
  void checkDuplicatePoints(Graph& graph)
  {
    FloatFloatIdMap map;

    IdPointMap::iterator it = graph.points.begin();
    while (it != graph.points.end())
    {
      FloatFloatIdMap::iterator itx = map.find(it->second.x);
      if (itx != map.end())
      {
        FloatIdMap::iterator ity = itx->second.find(it->second.y);
        if (ity != itx->second.end())
        {
          // duplicate
          printf("Warning: Duplicate point found %d: (%f, %f)\n", it->first, it->second.x, it->second.y);

          for (EdgeValueList::iterator eit = graph.edges.begin(); eit != graph.edges.end(); ++eit)
          {
            if (eit->first.first == it->first)
              eit->first.first = ity->second;

            if (eit->first.second == it->first)
              eit->first.second = ity->second;
          }

          it = graph.points.erase(it);
          continue;
        }

        itx->second.insert(FloatIdMap::value_type(it->second.y, it->first));
      }
      else
      {
        FloatIdMap fmap;
        fmap.insert(FloatIdMap::value_type(it->second.y, it->first));
        map.insert(FloatFloatIdMap::value_type(it->second.x, fmap));
      }

      ++it;
    }
  }

  bool checkEdgeSet(IdIdSetMap& map, int p1, int p2)
  {
    IdIdSetMap::iterator itp1 = map.find(p1);
    if (itp1 != map.end())
    {
      if (itp1->second.find(p2) != itp1->second.end())
        return true;

      itp1->second.insert(p2);
    }
    else
    {
      IdSet set;
      set.insert(p2);
      map.insert(IdIdSetMap::value_type(p1, set));
    }

    return false;
  }

  void checkDuplicateEdges(Graph& graph)
  {
    IdIdSetMap map;

    EdgeValueList::iterator it = graph.edges.begin();
    while (it != graph.edges.end())
    {
      if (checkEdgeSet(map, it->first.first, it->first.second) ||
        checkEdgeSet(map, it->first.second, it->first.first))
      {
        // duplicate
        printf("Warning: Duplicate edge found (%d, %d)\n", it->first.first, it->first.second);

        it = graph.edges.erase(it);
        continue;
      }
        
      ++it;
    }
  }

  void removeDuplicates(Graph& graph)
  {
    printf("check for duplicates\n");

    checkDuplicatePoints(graph);
    checkDuplicateEdges(graph);
  }

  void removeSinglePoints(Graph& graph)
  {
    printf("Check for single points\n");

    NeighbourMap neighbours;
    neighbourMapFromEdges(graph.edges, neighbours);

    IdPointMap::iterator nit = graph.points.begin();
    while (nit != graph.points.end())
    {
      if (neighbours.find(nit->first) == neighbours.end())
      {
        printf("Single point found: %d\n", nit->first);
        nit = graph.points.erase(nit);
      }
      else
      {
        ++nit;
      }
    }
  }

  void splitSingleEdges(Graph& graph)
  {
    std::set<std::set<int>> processedEdges;

    NeighbourMap neighbours;
    neighbourMapFromEdges(graph.edges, neighbours);

    // get current id
    int id = -1;
    for (IdPointMap::iterator it = graph.points.begin(); it != graph.points.end(); ++it)
      id = std::max(id, it->first);

    for (NeighbourMap::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
    {
      int pointId = it->first;
      if (it->second.size() > 2)
      {
        for (NeighbourValueList::iterator nit = it->second.begin(); nit != it->second.end(); ++nit)
        {
          int neighbourId = nit->first;
          uchar nvalue = nit->second;

          std::set<int> e;
          e.insert(pointId);
          e.insert(neighbourId);

          if (neighbours[neighbourId].size() > 2 && processedEdges.find(e) == processedEdges.end())
          {
            printf("Split edge: (%d, %d)\n", pointId, neighbourId);

            // found single egde that connects two nodals
            cv::Point2i p1 = graph.points[pointId];
            cv::Point2i p2 = graph.points[neighbourId];

            int newid = ++id;
            cv::Point2i pnew = (p1 + p2) / 2.0f;

            graph.points[newid] = pnew;

            graph.edges.push_back(EdgeValue(Edge(pointId, newid), nvalue));
            graph.edges.push_back(EdgeValue(Edge(newid, neighbourId), nvalue));

            // remove edge
            for (EdgeValueList::iterator eit = graph.edges.begin(); eit != graph.edges.end(); ++eit)
            {
              if ((eit->first.first == pointId && eit->first.second == neighbourId) ||
                (eit->first.first == neighbourId && eit->first.second == pointId))
              {
                graph.edges.erase(eit);
                break;
              }
            }

            processedEdges.insert(e);
          }
        }
      }
    }
  }
}