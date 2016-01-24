#include "findcycles.h"
#include <set>

namespace helper
{
  typedef std::pair<int, uchar> PointIdValue;
  typedef std::multimap<double, PointIdValue> AnglePointIdValueMap;
  typedef std::set<int> PointsVisited;

  typedef struct _cycleStage
  {
    int adjacentCycleIds[2];
    Edge directedEdge;
  } CycleStage;

  typedef std::vector<CycleStage> CycleStageList;

  double angleBetween(cv::Vec2f v1, cv::Vec2f v2)
  {
    double dot = v1.val[0] * v2.val[0] + v1.val[1] * v2.val[1];
    double det = v1.val[0] * v2.val[1] - v1.val[1] * v2.val[0];

    double angle = atan2(det, dot);
    if (angle < 0)
      angle += 2 * CV_PI;

    return angle;
  }

  void makeAnglePointMap(Graph& graph, NeighbourValueList& neighbours, int originId, int p1Id, AnglePointIdValueMap& angles, PointsVisited& visited)
  {
    cv::Vec2f p1;
    if (p1Id < 0)
      p1 = cv::Vec2f(-1.0f, 0.0f);
    else
      p1 = graph.points[p1Id] - graph.points[originId];

    for (NeighbourValueList::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
    {
      //if (visited.find(it->first) != visited.end())
      //  continue;

      if (it->first != p1Id)
        angles.insert(AnglePointIdValueMap::value_type(angleBetween(p1, graph.points[it->first] - graph.points[originId]), PointIdValue(it->first, it->second)));
    }
  }

  void makeNextStages(AnglePointIdValueMap& angles, NeighbourValueList& neighbours, CycleStageList& nextStage, CycleStageList& results, int& cycleId, PointsVisited& visited, int originId, PointsVisited& processed)
  {
    for (AnglePointIdValueMap::iterator it = angles.begin(); it != angles.end(); ++it)
    {
      if (processed.find(it->second.first) != processed.end())
        continue;

      CycleStage stage;

      stage.adjacentCycleIds[0] = cycleId;
      if (it->second.second == 255)
      {
        stage.adjacentCycleIds[1] = cycleId++;
      }
      else
      {
        stage.adjacentCycleIds[1] = -1;

        if (neighbours.size() > 2)
          cycleId++;
      }

      stage.directedEdge = Edge(originId, it->second.first);

      if (visited.find(it->second.first) == visited.end())
      {
        nextStage.push_back(stage);
        visited.insert(it->second.first);
      }

      results.push_back(stage);
    }

    processed.insert(originId);
  }

  void findCycles(Graph& graph, Cycles& cycles)
  {
    CycleStageList results;
    PointsVisited visited;
    NeighbourMap neighbourMap;
    neighbourMapFromGraph(graph, neighbourMap);

    while (results.size() < graph.edges.size())
    {
      // determine first stage
      NeighbourMap::iterator nit = neighbourMap.begin();
      //while ((nit != neighbourMap.end()) && (nit->second.size() < 1) && (visited.find(nit->first) != visited.end()))
      while ((nit->second.size() < 2) || (visited.find(nit->first) != visited.end()))
      {
        ++nit;
      }

      //if (nit == neighbourMap.end())
      //  break;

      int cycleId = 0;

      visited.insert(nit->first);

      AnglePointIdValueMap angles;
      makeAnglePointMap(graph, nit->second, nit->first, -1, angles, visited);

      CycleStageList currentStage;
      PointsVisited processed;
      makeNextStages(angles, nit->second, currentStage, results, cycleId, visited, nit->first, processed);

      while (currentStage.size() > 0)
      {
        CycleStageList nextStage;

        for (CycleStageList::iterator sit = currentStage.begin(); sit != currentStage.end(); ++sit)
        {
          NeighbourValueList nextNeighbours = neighbourMap.find(sit->directedEdge.second)->second;

          AnglePointIdValueMap nextAngles;
          makeAnglePointMap(graph, nextNeighbours, sit->directedEdge.second, sit->directedEdge.first, nextAngles, visited);
          makeNextStages(nextAngles, nextNeighbours, nextStage, results, cycleId, visited, sit->directedEdge.second, processed);
        }

        currentStage = nextStage;
      }

    }

    return;
  }
}