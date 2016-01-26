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
    
  }

  template<typename Container>
  typename Container::iterator nextContainerElement(typename Container::iterator& it)
  {
    typename Container::iterator next = it;
    ++next;

    return next;
  }

  template<typename Container>
  bool isLastContainerElement(Container& container, typename Container::iterator& it)
  {
    typename Container::iterator next = it;
    ++next;

    return next == container.end();
  }

  void setAdjacentCycle(CycleStage& stage, Side side, int cycleId)
  {
    if (stage.isBoundary && side == RIGHT)
      side = LEFT;

    stage.adjacentCycleIds[side] = cycleId;
  }

  int getAdjacentCycle(CycleStage& stage, Side side)
  {
    if (stage.isBoundary && side == RIGHT)
      side = LEFT;

    return stage.adjacentCycleIds[side];
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

    AnglePointIdValueMap::iterator ait = angles.begin();
    if (isFirst)
    {
      leftCycleId = cycleId++;

      if (ait->second.second == 255)
        rightCycleId = cycleId++;

      makeStage(ait->second, visited, originId, leftCycleId, rightCycleId, results, nextStage);
      ++ait;
    }

    int currentCycleId = leftCycleId;
    if (rightCycleId == -1)
      rightCycleId = leftCycleId;

    for (ait; ait != angles.end(); ++ait)
    {
      AnglePointIdValueMap::iterator nextit = nextContainerElement<AnglePointIdValueMap>(ait);
      bool isLast = isLastContainerElement(angles, ait);

      if (processed.find(ait->second.first) != processed.end())
      {
        continue;
      }

      makeStage(ait->second, visited, originId, leftCycleId, rightCycleId, nextStage, results);

      //CycleStage stage;

      /*stage.adjacentCycleIds[0] = cycleId;
      if (it->second.second == 255)
      {
        stage.adjacentCycleIds[1] = cycleId++;
      }
      else
      {
        stage.adjacentCycleIds[1] = -1;

        if (neighbours.size() > 2)
          cycleId++;
      }*/

      /*stage.adjacentCycleIds[0] = currentCycleId;

      if (ait->second.second == 255)
      {
        // last element
        //if (it->second.first == angles.rbegin()->second.first)
        if (isLast)
        {
          //if (rightCycleId == -1)
          //  stage.adjacentCycleIds[1] = 
          //stage.adjacentCycleIds[1] = rightCycleId == -1 ? ;
        }
        else
        {
          stage.adjacentCycleIds[1] = cycleId++;
          currentCycleId = stage.adjacentCycleIds[1];
        }
      }
      else
      {
        stage.adjacentCycleIds[1] = -1;

        //if (neighbours.size() > 2)
        //  cycleId++;
      }

      stage.directedEdge = Edge(originId, ait->second.first);
      stage.isBoundary = ait->second.second < 255;

      if (visited.find(ait->second.first) == visited.end())
      {
        nextStage.push_back(stage);
        visited.insert(ait->second.first);
      }

      results.push_back(stage);*/


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
      while ((nit->second.size() < 2) || (visited.find(nit->first) != visited.end()))
      {
        ++nit;
      }

      int cycleId = 0;

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

    return;
  }
}