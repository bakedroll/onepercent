#include "findcycles.h"

#include "draw.h"
#include "quadtree.h"

#include <set>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace helper
{
  typedef struct _cycleStage
  {
    int adjacentCycleIds[2];
    Edge directedEdge;
    bool isBoundary;
  } CycleStage;

  typedef std::vector<CycleStage> CycleStageList;

  void addContourPointId(Graph& graph, PointSet& boundingPoints, PointList& contour, int pointId)
  {
    contour.push_back(graph.points.find(pointId)->second);
    boundingPoints.insert(pointId);
  }

  bool pointInCycle(Graph& graph, PointSet& bounds, PointList& contour, int pointId)
  {
    if (bounds.find(pointId) != bounds.end() ||
      cv::pointPolygonTest(contour, graph.points.find(pointId)->second, false) >= 0)
      return true;

    return false;
  }

  void addCycleTriangles(Graph& graph, Quadtree<int>& quadtree, PointTriangleMap& triangles, Cycle& cycle)
  {
    if (cycle.edges.size() < 1)
      return;

    PointSet boundingPoints;
    PointList contour;
    NeighbourMap neighbourMap;

    neighbourMapFromEdges(cycle.edges, neighbourMap);

    NeighbourMap::iterator eit = neighbourMap.begin();
    addContourPointId(graph, boundingPoints, contour, eit->first);

    for (int i = 0; i < int(cycle.edges.size()); i++)
    //while (contour.size() < cycle.edges.size())
    {
      for (NeighbourValueList::iterator it = eit->second.begin(); it != eit->second.end(); ++it)
      {
        if (boundingPoints.find(it->first) == boundingPoints.end())
        {
          addContourPointId(graph, boundingPoints, contour, it->first);
          eit = neighbourMap.find(it->first);
          break;
        }
      }
    }

    BoundingBox bb(graph, boundingPoints);
    cv::Point center = bb.center();
    AABB aabb(Point(float(center.x), float(center.y)), Point(float(bb.width() / 2 + 5), float(bb.height() / 2 + 5)));

    std::vector<Data<int>> points = quadtree.queryRange(aabb);

    IdSet trianglesVisited;

    /*for (std::vector<Data<int>>::iterator it = points.begin(); it != points.end(); ++it)
    {
      PointTriangleMap::iterator tit = triangles.find(*it->load);
      for (IdSet::iterator iit = tit->second.begin(); iit != tit->second.end(); ++iit)
      {
        if (trianglesVisited.find(*iit) != trianglesVisited.end())
          continue;

        TriangleMap::iterator tmit = graph.triangles.find(*iit);
        if (pointInCycle(graph, boundingPoints, contour, tmit->second.idx[0]) &&
          pointInCycle(graph, boundingPoints, contour, tmit->second.idx[1]) &&
          pointInCycle(graph, boundingPoints, contour, tmit->second.idx[2]))
        {
          cycle.trianlges.insert(TriangleMap::value_type(tmit->first, tmit->second));
        }

        trianglesVisited.insert(*iit);
      }
    }*/

    for (TriangleMap::iterator it = graph.triangles.begin(); it != graph.triangles.end(); ++it)
    {
      if (pointInCycle(graph, boundingPoints, contour, it->second.idx[0]) &&
        pointInCycle(graph, boundingPoints, contour, it->second.idx[1]) &&
        pointInCycle(graph, boundingPoints, contour, it->second.idx[2]))
      {
        cycle.trianlges.insert(*it);
      }
    }
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

  void makeAnglePointMap(Graph& graph, NeighbourValueList& neighbours, int originId, int p1Id, AnglePointIdValueMap& angles, PointSet& visited)
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

        if (leftId == -1)
          std::swap(leftId, rightId);

        return;
      }
      /*if (it->directedEdge.first == p1 && it->directedEdge.second == p2)
      {
        leftId = it->adjacentCycleIds[0];
        rightId = it->adjacentCycleIds[1];

        if (leftId == -1)
          std::swap(leftId, rightId);

        return;
      } */     
    }

    leftId = -1;
    rightId = -1;
  }

  template<typename Container>
  bool isLastContainerElement(Container& container, typename Container::iterator& it)
  {
    typename Container::iterator next = it;
    ++next;

    return next == container.end();
  }

  bool makeStage(
    PointIdValue& pvalue,
    PointSet& visited,
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

    results.push_back(stage);

    if (visited.find(pvalue.first) == visited.end())
    {
      nextStage.push_back(stage);
      visited.insert(pvalue.first);

      return false;
    }

    return true;
  }

  void mergeCycleIds(CycleStageList& stage, int cycleId, int replaceById)
  {
    if (cycleId == replaceById)
      return;

    for (CycleStageList::iterator it = stage.begin(); it != stage.end(); ++it)
    {
      for (int i = 0; i < 2; i++)
        if (it->adjacentCycleIds[i] == cycleId)
          it->adjacentCycleIds[i] = replaceById;
    }
  }

  void makeNextStages(
    Graph& graph,
    AnglePointIdValueMap& angles,
    NeighbourMap& neighbours,
    CycleStageList& nextStage,
    CycleStageList& currentStage,
    CycleStageList& results,
    int& cycleId,
    PointSet& visited,
    int originId,
    PointSet& processed,
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
        getLeftRightCycleId(results, originId, ait->second.first, l, r);

        prevBoundary = ait->second.second < 255;
        if (prevBoundary)
          currentCycleId = l;
        else
          currentCycleId = r > -1 ? r : l;

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

      if (makeStage(ait->second, visited, originId, left, right, nextStage, results))
      {
        NeighbourMap::iterator nit = neighbours.find(ait->second.first);

        // merge
        AnglePointIdValueMap pangles;
        makeAnglePointMap(graph, nit->second, ait->second.first, originId, pangles, visited);

        assert(pangles.size() > 0);

        AnglePointIdValueMap::iterator lit = pangles.begin();
        AnglePointIdValueMap::reverse_iterator rit = pangles.rbegin();

        int l, r;
        getLeftRightCycleId(results, ait->second.first, lit->second.first, l, r);
        if (l > -1)
        {
          mergeCycleIds(results, left, l);
          mergeCycleIds(currentStage, left, l);
          mergeCycleIds(nextStage, left, l);
        }

        if (ait->second.second == 255)
        {
          getLeftRightCycleId(results, ait->second.first, rit->second.first, l, r);

          if (r > -1)
          {
            mergeCycleIds(results, right, r);
            mergeCycleIds(currentStage, right, r);
            mergeCycleIds(nextStage, right, r);
          }
        }

      }
    }

    processed.insert(originId);
  }

  void normalize(cv::Vec2f& p)
  {
    float l = sqrt(p.val[0] * p.val[0] + p.val[1] * p.val[1]);
    p.val[0] /= l;
    p.val[1] /= l;
  }

  void drawAdjacentCycleNumbers(cv::Mat& image, cv::Point2f& p1, cv::Point2f& p2, int left, int right, float scale)
  {
    cv::Vec2f mid((p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f);

    cv::Vec2f p1vec = cv::Vec2f(p1) - mid;
    cv::Vec2f p2vec = cv::Vec2f(p2) - mid;

    normalize(p1vec);
    normalize(p2vec);

    cv::Vec2f r = cv::Vec2f(p1vec.val[1], -p1vec.val[0]) * 10.0f + mid;
    cv::Vec2f l = cv::Vec2f(p2vec.val[1], -p2vec.val[0]) * 10.0f + mid;

    cv::putText(image, std::to_string(left), cv::Point(int(l[0]), int(l[1])) * scale, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255));
    cv::putText(image, std::to_string(right), cv::Point(int(r[0]), int(r[1])) * scale, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255));
  }

  void debugStage(cv::Mat& image, Graph& graph, CycleStageList& results, CycleStageList& stage, float scale)
  {
    drawGraph(image, graph, scale, false);

    for (CycleStageList::iterator it = results.begin(); it != results.end(); ++it)
    {
      drawEdge(image, graph, it->directedEdge.first, it->directedEdge.second, scale, cv::Scalar(0, 150, 0));
    }

    for (CycleStageList::iterator it = stage.begin(); it != stage.end(); ++it)
    {
      IdPointMap::iterator itp1 = graph.points.find(it->directedEdge.first);
      IdPointMap::iterator itp2 = graph.points.find(it->directedEdge.second);

      cv::circle(image, itp1->second * scale, 5, cvScalar(0, 50, 255), -1);
      cv::circle(image, itp2->second * scale, 3, cvScalar(0, 255, 255), -1);

      drawAdjacentCycleNumbers(image, itp1->second, itp2->second, it->adjacentCycleIds[0], it->adjacentCycleIds[1], scale);
    }

    cv::imshow("Debug Cycles", image);
    cv::waitKey(600000);
  }

  void findCycles(Graph& graph, Cycles& cycles, bool debug, int rows, int cols, float scale)
  {
    cv::Mat debugImage;
    if (debug)
      debugImage = cv::Mat(int(rows * scale), int(cols * scale), CV_8UC3);

    CycleStageList results;
    PointSet visited;
    NeighbourMap neighbourMap;
    neighbourMapFromEdges(graph.edges, neighbourMap);

    int cycleId = 0;

    while (results.size() < graph.edges.size())
    {
      // determine first stage
      NeighbourMap::iterator nit = neighbourMap.begin();
      while (nit != neighbourMap.end() && ((nit->second.size() < 2) || (visited.find(nit->first) != visited.end())))
      {
        ++nit;
      }

      // safe
      if (nit == neighbourMap.end())
        break;

      visited.insert(nit->first);

      AnglePointIdValueMap angles;
      makeAnglePointMap(graph, nit->second, nit->first, -1, angles, visited);

      CycleStageList currentStage;
      PointSet processed;
      makeNextStages(graph, angles, neighbourMap, currentStage, currentStage, results, cycleId, visited, nit->first, processed, -1, -1, true);

      while (currentStage.size() > 0)
      {
        if (debug)
          debugStage(debugImage, graph, results, currentStage, scale);

        CycleStageList nextStage;

        for (CycleStageList::iterator sit = currentStage.begin(); sit != currentStage.end(); ++sit)
        {
          NeighbourValueList nextNeighbours = neighbourMap.find(sit->directedEdge.second)->second;

          AnglePointIdValueMap nextAngles;
          makeAnglePointMap(graph, nextNeighbours, sit->directedEdge.second, sit->directedEdge.first, nextAngles, visited);
          makeNextStages(
            graph,
            nextAngles,
            neighbourMap,
            nextStage,
            currentStage,
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

    Quadtree<int> quadtree;
    for (IdPointMap::iterator it = graph.points.begin(); it != graph.points.end(); ++it)
      quadtree.insert(Data<int>(Point(it->second.x, it->second.y), const_cast<int*>(&it->first)));

    PointTriangleMap triangleMap;
    makePointTriangleMap(graph, triangleMap);

    for (int cId = 0; cId < cycleId; cId++)
    {
      Cycle cycle;
      for (CycleStageList::iterator it = results.begin(); it != results.end(); ++it)
      {
        if (it->adjacentCycleIds[0] == cId || it->adjacentCycleIds[1] == cId)
          cycle.edges.push_back(EdgeValue(Edge(it->directedEdge.first, it->directedEdge.second), it->isBoundary ? 128 : 255));
      }

      if (cycle.edges.size() > 0)
      {
        addCycleTriangles(graph, quadtree, triangleMap, cycle);

        cycles.push_back(cycle);
      }
    }
  }
}