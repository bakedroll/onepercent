#include "vectorizer/findcycles.h"

#include "vectorizer/draw.h"
#include "vectorizer/io.h"
#include "vectorizer/quadtree.h"

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

  void addContourPointId(Graph& graph, IdSet& boundingPoints, PointListi& contour, int pointId)
  {
    contour.push_back(graph.points.find(pointId)->second);
    boundingPoints.insert(pointId);
  }

  void addCycleTriangles(Graph& graph, QuadTreeNode<int>& quadtree, IdSet& trianglesVisited, Cycle& cycle)
  {
    if (cycle.edges.size() < 1)
      return;

    IdSet boundingPoints;
    PointListi contour;
    NeighbourMap neighbourMap;

    neighbourMapFromEdges(cycle.edges, neighbourMap);

    NeighbourMap::iterator eit = neighbourMap.begin();
    addContourPointId(graph, boundingPoints, contour, eit->first);

    int nEdges = 0;
    for (NeighbourEdgesMap::iterator it = cycle.edges.begin(); it != cycle.edges.end(); ++it)
      nEdges += int(it->second.size());

    for (int i = 0; i < nEdges; i++)
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

    PointListf points;
    makePointList(graph.points, boundingPoints, points);
    BoundingBox<float> bb(points);

    cycle.boundingbox = bb;

    QuadTreeNode<int>::Data trianglesCheck;
    quadtree.gatherDataWithinBoundary(bb, trianglesCheck);

    for (QuadTreeNode<int>::Data::iterator it = trianglesCheck.begin(); it != trianglesCheck.end(); ++it)
    {
      if (trianglesVisited.find(it->data()) == trianglesVisited.end() &&
        cv::pointPolygonTest(contour, it->point(), false) >= 0)
      {
        cycle.trianlges.insert(*graph.triangles.find(it->data()));
        trianglesVisited.insert(it->data());
      }
    }
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
    IdSet& visited,
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
    IdSet& visited,
    int originId,
    IdSet& processed,
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
        makeAnglePointMap(graph.points, nit->second, ait->second.first, originId, pangles);

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
    drawGraph(image, graph, scale, false, true);

    for (CycleStageList::iterator it = results.begin(); it != results.end(); ++it)
    {
      drawEdge(image, graph, it->directedEdge.first, it->directedEdge.second, scale, cv::Scalar(0, 150, 0));
    }

    for (CycleStageList::iterator it = stage.begin(); it != stage.end(); ++it)
    {
      IdPointMap::iterator itp1 = graph.points.find(it->directedEdge.first);
      IdPointMap::iterator itp2 = graph.points.find(it->directedEdge.second);

      cv::circle(image, itp1->second * scale, 5, cv::Scalar(0, 50, 255), -1);
      cv::circle(image, itp2->second * scale, 3, cv::Scalar(0, 255, 255), -1);

      drawAdjacentCycleNumbers(image, itp1->second, itp2->second, it->adjacentCycleIds[0], it->adjacentCycleIds[1], scale);
    }

    cv::imshow("Debug Cycles", image);
    cv::waitKey(600000);
  }

  void assignTriangles(Graph& graph, CyclesMap& cycles, int cycleId, CycleStageList& results)
  {
    ProgressPrinter qprogress("Build quadtree");

    QuadTreeNode<int> quadtree(graph.boundary, 10);

    int i = 0;
    int max = graph.triangles.size();
    for (TriangleMap::iterator it = graph.triangles.begin(); it != graph.triangles.end(); ++it)
    {
      cv::Point2f p1 = graph.points.find(it->second.idx[0])->second;
      cv::Point2f p2 = graph.points.find(it->second.idx[1])->second;
      cv::Point2f p3 = graph.points.find(it->second.idx[2])->second;

      quadtree.insert(QuadTreeNodeData<int>(cv::Point2f((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f), it->first));

      i++;
      qprogress.update(i, max);
    }

    ProgressPrinter aprogress("Assign triangles to cycles");
    IdSet trianglesVisited;

    for (int cId = 0; cId < cycleId; cId++)
    {
      Cycle cycle;
      for (CycleStageList::iterator it = results.begin(); it != results.end(); ++it)
      {
        for (int j = 0; j < 2; j++)
        {
          if (it->adjacentCycleIds[j] == cId)
          {
            NeighbourEdgesMap::iterator nit = cycle.edges.find(it->adjacentCycleIds[1 - j]);
            if (nit == cycle.edges.end())
            {
              EdgeList edges;
              edges.push_back(Edge(it->directedEdge.first, it->directedEdge.second));
              cycle.edges.insert(NeighbourEdgesMap::value_type(it->adjacentCycleIds[1 - j], edges));
            }
            else
            {
              nit->second.push_back(Edge(it->directedEdge.first, it->directedEdge.second));
            }

            break;
          }
        }
      }

      if (cycle.edges.size() > 0)
      {
        addCycleTriangles(graph, quadtree, trianglesVisited, cycle);

        if (cycle.trianlges.size() > 0)
          cycles.insert(CyclesMap::value_type(cId, cycle));
      }

      aprogress.update(cId, cycleId - 1);
    }
  }

  void findCycles(Graph& graph, CyclesMap& cycles, bool debug, float scale)
  {
    ProgressPrinter progress("Detect cylces");

    cv::Mat debugImage;
    if (debug)
      debugImage = cv::Mat(int(graph.boundary.height() * scale), int(graph.boundary.width() * scale), CV_8UC3);

    CycleStageList results;
    IdSet visited;
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
      makeAnglePointMap(graph.points, nit->second, nit->first, -1, angles);

      CycleStageList currentStage;
      IdSet processed;
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
          makeAnglePointMap(graph.points, nextNeighbours, sit->directedEdge.second, sit->directedEdge.first, nextAngles);
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

      progress.update(results.size(), graph.edges.size());
    }

    assignTriangles(graph, cycles, cycleId, results);
  }
}