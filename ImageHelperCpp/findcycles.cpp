#include "findcycles.h"
#include <set>

namespace helper
{
  typedef std::pair<int, uchar> PointIdValue;
  typedef std::multimap<double, PointIdValue> AnglePointIdValueMap;
  typedef std::set<int> PointsVisited;

  typedef enum
  {
    LEFT = 0,
    RIGHT = 1
  } Side;

  typedef struct _cycleStage
  {
    int adjacentCycleIds[2];
    Edge directedEdge;
    bool isBoundary;
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
      if (it->first != p1Id)
        angles.insert(AnglePointIdValueMap::value_type(angleBetween(p1, graph.points[it->first] - graph.points[originId]), PointIdValue(it->first, it->second)));
  }

  void getLeftRightCycleId(CycleStageList& results, int p1, int p2, int& leftId, int& rightId)
  {
    for (CycleStageList::iterator it = results.begin(); it != results.end(); ++it)
    {
      if (it->directedEdge.first == p2 && it->directedEdge.second == p1)
      {
        leftId = it->adjacentCycleIds[1];
        rightId = it->adjacentCycleIds[0];
        return;
      }
    }

    assert(false);
  }

  template<typename Container>
  bool isLastContainerElement(Container& container, typename Container::iterator& it)
  {
    typename Container::iterator next = it;
    ++next;

    return next == container.end();
  }

  void makeStage(
    PointIdValue& pvalue,
    PointsVisited& visited,
    int origin,
    int left,
    int right,
    CycleStageList& nextStage,
    CycleStageList& results)
  {
    CycleStage stage;

    stage.adjacentCycleIds[0] = left;
    stage.adjacentCycleIds[1] = right;

    stage.directedEdge = Edge(origin, pvalue.first);
    stage.isBoundary = pvalue.second < 255;

    if (visited.find(pvalue.first) == visited.end())
    {
      nextStage.push_back(stage);
      visited.insert(pvalue.first);
    }

    results.push_back(stage);
  }

  void getLeftRight(CycleStageList& results, int originId, int targetId, int& left, int& right)
  {
    
  }

  void makeNextStages(
    AnglePointIdValueMap& angles,
    NeighbourValueList& neighbours,
    CycleStageList& nextStage,
    CycleStageList& results,
    int& cycleId,
    PointsVisited& visited,
    int originId,
    PointsVisited& processed,
    int leftCycleId = -1,
    int rightCycleId = -1,
    bool isFirst = false)
  {
    if (angles.size() == 0)
      return;

    bool prevBoundary;

    AnglePointIdValueMap::iterator ait = angles.begin();
    if (isFirst)
    {
      leftCycleId = cycleId++;

      prevBoundary = ait->second.second < 255;
      if (ait->second.second == 255)
        rightCycleId = cycleId++;

      makeStage(ait->second, visited, originId, leftCycleId, rightCycleId, results, nextStage);
      ++ait;
    }
    else
    {
      prevBoundary = rightCycleId == -1;
    }

    int currentCycleId = leftCycleId;
    if (rightCycleId == -1)
      rightCycleId = leftCycleId;

    for (ait; ait != angles.end(); ++ait)
    {
      // edge already processed
      if (processed.find(ait->second.first) != processed.end())
      {
        int l, r;
        getLeftRight(results, originId, ait->second.first, l, r);

        prevBoundary = ait->second.second < 255;
        if (prevBoundary)
          currentCycleId = l;
        else
          currentCycleId = r;

        continue;
      }

      bool isLast = isLastContainerElement(angles, ait);
      int left, right;

      left = currentCycleId;
      if (ait->second.second < 255)
      {
        if (isLast)
        {
          if (prevBoundary)
            left = rightCycleId;
        }
        else
        {
          if (prevBoundary)
          {
            left = cycleId++;
            currentCycleId = left;
          }
        }

        right = -1;

        prevBoundary = true;
      }
      else
      {
        if (isLast)
        {
          right = rightCycleId;
        }
        else
        {
          right = cycleId++;
          currentCycleId = right;
        }

        prevBoundary = false;
      }

      makeStage(ait->second, visited, originId, left, right, nextStage, results);
    }

    processed.insert(originId);
  }

  void findCycles(Graph& graph, Cycles& cycles)
  {
    CycleStageList results;
    PointsVisited visited;
    NeighbourMap neighbourMap;
    neighbourMapFromGraph(graph, neighbourMap);

    int cycleId = 0;

    while (results.size() < graph.edges.size())
    {
      // determine first stage
      NeighbourMap::iterator nit = neighbourMap.begin();
      while ((nit->second.size() < 2) || (visited.find(nit->first) != visited.end()))
      {
        ++nit;
      }

      visited.insert(nit->first);

      AnglePointIdValueMap angles;
      makeAnglePointMap(graph, nit->second, nit->first, -1, angles, visited);

      CycleStageList currentStage;
      PointsVisited processed;
      makeNextStages(angles, nit->second, currentStage, results, cycleId, visited, nit->first, processed, -1, -1, true);

      while (currentStage.size() > 0)
      {
        CycleStageList nextStage;

        for (CycleStageList::iterator sit = currentStage.begin(); sit != currentStage.end(); ++sit)
        {
          NeighbourValueList nextNeighbours = neighbourMap.find(sit->directedEdge.second)->second;

          AnglePointIdValueMap nextAngles;
          makeAnglePointMap(graph, nextNeighbours, sit->directedEdge.second, sit->directedEdge.first, nextAngles, visited);
          makeNextStages(
            nextAngles,
            nextNeighbours,
            nextStage,
            results,
            cycleId,
            visited,
            sit->directedEdge.second,
            processed,
            sit->adjacentCycleIds[0],
            sit->adjacentCycleIds[1]);
        }

        currentStage = nextStage;
      }

    }

    for (int cId = 0; cId < cycleId; cId++)
    {
      if (cId != 3)
        continue;

      EdgeValueList cycle;
      for (CycleStageList::iterator it = results.begin(); it != results.end(); ++it)
      {
        if (it->adjacentCycleIds[0] == cId || it->adjacentCycleIds[2] == cId)
          cycle.push_back(EdgeValue(Edge(it->directedEdge.first, it->directedEdge.second), it->isBoundary ? 128 : 255));
      }

      if (cycle.size() > 0)
        cycles.push_back(cycle);
    }
  }
}