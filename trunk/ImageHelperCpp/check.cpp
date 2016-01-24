#include "check.h"

namespace helper
{
  void checkPointAndRemove(Graph& graph, IdPointMap::iterator& pointIt)
  {
    int count = 0;

    IdPointMap::iterator it = pointIt;
    ++it;
    while (it != graph.points.end())
    {
      if (it->second == pointIt->second)
      {
        count++;

        for (EdgeValueList::iterator eit = graph.edges.begin(); eit != graph.edges.end(); ++eit)
        {
          if (eit->first.first == it->first)
            eit->first.first = pointIt->first;

          if (eit->first.second == it->first)
            eit->first.second = pointIt->first;
        }

        it = graph.points.erase(it);
      }
      else
      {
        ++it;
      }
    }

    if (count > 0)
      printf("Duplicate point: %d\n", pointIt->first);
  }

  void checkEdgeAndRemove(Graph& graph, EdgeValueList::iterator& edgeIt)
  {
    int count = 0;

    EdgeValueList::iterator it = edgeIt;
    ++it;
    while (it != graph.edges.end())
    {
      if ((it->first.first == edgeIt->first.first && it->first.second == edgeIt->first.second) ||
        (it->first.first == edgeIt->first.second && it->first.second == edgeIt->first.first))
      {
        count++;

        it = graph.edges.erase(it);
      }
      else
      {
        ++it;
      }
    }

    if (count > 0)
      printf("Duplicate edge: (%d, %d)\n", edgeIt->first.first, edgeIt->first.second);
  }

  void checkDuplicatesAndRemove(Graph& graph)
  {
    printf("checking for duplicates...\n");

    for (IdPointMap::iterator it = graph.points.begin(); it != graph.points.end(); ++it)
      checkPointAndRemove(graph, it);

    for (EdgeValueList::iterator it = graph.edges.begin(); it != graph.edges.end(); ++it)
      checkEdgeAndRemove(graph, it);

    printf("\n");
  }
}