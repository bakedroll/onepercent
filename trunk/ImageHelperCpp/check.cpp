#include "check.h"

namespace helper
{
  void checkPoint(Graph& graph, IdPointMap::iterator& pointIt)
  {
    int count = 0;

    for (IdPointMap::iterator it = graph.points.begin(); it != graph.points.end(); ++it)
      if (it->second == pointIt->second)
        count++;

    if (count != 1)
      printf("Duplicate point: %d\n", pointIt->first);
  }

  void checkEdge(Graph& graph, EdgeValueList::iterator& edgeIt)
  {
    int count = 0;

    for (EdgeValueList::iterator it = graph.edges.begin(); it != graph.edges.end(); ++it)
      if ((it->first.first == edgeIt->first.first && it->first.second == edgeIt->first.second) ||
        (it->first.first == edgeIt->first.second && it->first.second == edgeIt->first.first))
        count++;

    if (count != 1)
      printf("Duplicate edge: (%d, %d)\n", edgeIt->first.first, edgeIt->first.second);
  }

  void checkDuplicates(Graph& graph)
  {
    printf("checking for duplicates...\n");

    for (IdPointMap::iterator it = graph.points.begin(); it != graph.points.end(); ++it)
      checkPoint(graph, it);

    for (EdgeValueList::iterator it = graph.edges.begin(); it != graph.edges.end(); ++it)
      checkEdge(graph, it);

    printf("\n");
  }
}