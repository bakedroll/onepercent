#include "detect.h"
#include "io.h"

namespace helper
{
  typedef struct _stage
  {
    PointListi points;
    int iteration;
    int currentId;
    uchar edgeValue;
    bool obsolete;
  } Stage;

  typedef std::vector<Stage> StageList;

  cv::Vec3f randomColor()
  {
    return cv::Vec3b(rand() % 100 + 155, rand() % 100 + 155, rand() % 100 + 155);
  }

  void checkPixel(cv::Mat& image, BoolArray& aVisited, IntArray* aPointIds, PointValueList& points, int* neighbourId, int x, int y, bool repeat)
  {
    if (x < 0) if (repeat) x += image.cols; else return;
    if (y < 0) if (repeat) y += image.rows; else return;
    if (x >= image.cols) if (repeat) x -= image.cols; else return;
    if (y >= image.rows) if (repeat) y -= image.rows; else return;

    uchar pval = image.at<uchar>(cv::Point(x, y));
    if (pval > 0)
    {
      if (!aVisited.get(x, y))
      {
        points.push_back(PointValue(cv::Point(x, y), pval));
        aVisited.set(x, y, true);
      }
      else if (aPointIds != nullptr && aPointIds->get(x, y) > -1)
      {
        *neighbourId = aPointIds->get(x, y);
      }
    }
  }

  void checkPixelsAround(cv::Mat& image, BoolArray& aVisited, IntArray* aPointIds, PointValueList& points, int* neighbourId, int x, int y, bool repeat = true)
  {
    if (neighbourId != nullptr)
      *neighbourId = -1;

    checkPixel(image, aVisited, aPointIds, points, neighbourId, x - 1, y - 1, repeat);
    checkPixel(image, aVisited, aPointIds, points, neighbourId, x, y - 1, repeat);
    checkPixel(image, aVisited, aPointIds, points, neighbourId, x + 1, y - 1, repeat);
    checkPixel(image, aVisited, aPointIds, points, neighbourId, x - 1, y, repeat);
    checkPixel(image, aVisited, aPointIds, points, neighbourId, x + 1, y, repeat);
    checkPixel(image, aVisited, aPointIds, points, neighbourId, x - 1, y + 1, repeat);
    checkPixel(image, aVisited, aPointIds, points, neighbourId, x, y + 1, repeat);
    checkPixel(image, aVisited, aPointIds, points, neighbourId, x + 1, y + 1, repeat);
  }

  PointListi fromPointValueList(PointValueList& list)
  {
    PointListi result;
    for (PointValueList::iterator it = list.begin(); it != list.end(); ++it)
      result.push_back(it->first);

    return result;
  }

  void groupPoints(PointValueList& points, PointValueListGroup& groups)
  {
    PointListi pointList = fromPointValueList(points);

    BoundingBox<int> fieldAabb(pointList);
    cv::Mat field(fieldAabb.height(), fieldAabb.width(), CV_8UC1);
    BoolArray visited(fieldAabb.width(), fieldAabb.height(), false);

    field.setTo(0);

    for (PointValueList::iterator it = points.begin(); it != points.end(); ++it)
      field.at<uchar>(it->first - fieldAabb.min()) = it->second;

    int xmax = fieldAabb.width();
    int ymax = fieldAabb.height();
    for (int y = 0; y < ymax; y++)
    {
      for (int x = 0; x < xmax; x++)
      {
        uchar pval = field.at<uchar>(cv::Point(x, y));
        if (pval > 0 && !visited.get(x, y))
        {
          PointValueList group;

          PointValueList check;
          check.push_back(PointValue(cv::Point(x, y), pval));
          group.push_back(PointValue(cv::Point(x, y) + fieldAabb.min(), pval));

          visited.set(x, y, true);

          do
          {
            PointValueList result;
            for (PointValueList::iterator it = check.begin(); it != check.end(); ++it)
              checkPixelsAround(field, visited, nullptr, result, nullptr, it->first.x, it->first.y, false);

            for (PointValueList::iterator it = result.begin(); it != result.end(); ++it)
              group.push_back(PointValue(it->first + fieldAabb.min(), it->second));

            check = result;
          } while (check.size() > 0);

          groups.push_back(group);
        }
      }
    }
  }

  void addToResults(int& idCounter, int connectTo, IntArray& aPointIds, Graph& outGraph, const PointListi& points, uchar edgeVal = 255)
  {
    cv::Point2f p(0, 0);

    for (PointListi::const_iterator pIt = points.cbegin(); pIt != points.cend(); ++pIt)
    {
      aPointIds.set(pIt->x, pIt->y, idCounter);
      p += cv::Point2f(float(pIt->x), float(pIt->y));
    }

    p.x /= float(points.size());
    p.y /= float(points.size());

    outGraph.points.insert(IdPointMap::value_type(idCounter, p));

    if (connectTo > -1)
      outGraph.edges.push_back(EdgeValue(Edge(connectTo, idCounter), edgeVal));

    idCounter++;
  }

  bool stagesCollide(Stage& stage, Stage& next)
  {
    for (PointListi::iterator sit = stage.points.begin(); sit != stage.points.end(); ++sit)
      for (PointListi::iterator nit = next.points.begin(); nit != next.points.end(); ++nit)
        if (abs(sit->x - nit->x) <= 1 && abs(sit->y - nit->y) <= 1)
          return true;

    return false;
  }

  bool checkStagesCollide(Stage& stage, StageList& nextStage, int& idCounter, IntArray& aPointIds, Graph& outGraph)
  {
    for (StageList::iterator it = nextStage.begin(); it != nextStage.end(); ++it)
    {
      if ((&*it != &stage) && stagesCollide(stage, *it))
      {
        addToResults(idCounter, stage.currentId, aPointIds, outGraph, stage.points, stage.edgeValue);
        outGraph.edges.push_back(EdgeValue(Edge(idCounter - 1, it->currentId), it->edgeValue));

        it->obsolete = true;
        stage.iteration = 0;
        stage.currentId = idCounter - 1;

        return true;
      }
    }

    return false;
  }

  void checkEndOfLine(Stage& stage, StageList& nextStage, int& idCounter, IntArray& aPointIds, Graph& outGraph)
  {
    if (!checkStagesCollide(stage, nextStage, idCounter, aPointIds, outGraph))
      addToResults(idCounter, stage.currentId, aPointIds, outGraph, stage.points, stage.edgeValue);
  }

  void findNeighbours(cv::Mat& image, cv::Mat& display, BoolArray& aVisited, IntArray& aPointIds, Graph& outGraph, int x, int y, uchar edgeVal, int depth, int& idCounter)
  {
    StageList lCurrentStage;

    PointListi points;
    points.push_back(cv::Point(x, y));

    Stage firstStage;
    firstStage.points = points;
    firstStage.iteration = 0;
    firstStage.currentId = idCounter;
    firstStage.edgeValue = edgeVal;
    firstStage.obsolete = false;

    addToResults(idCounter, -1, aPointIds, outGraph, points);

    lCurrentStage.push_back(firstStage);
    aVisited.set(x, y, true);

    do
    {
      StageList lNextStage;

      for (StageList::iterator csIt = lCurrentStage.begin(); csIt != lCurrentStage.end(); ++csIt)
      {
        PointValueList neighbours;
        int neighbourId;

        for (PointListi::iterator pIt = csIt->points.begin(); pIt != csIt->points.end(); ++pIt)
          checkPixelsAround(image, aVisited, &aPointIds, neighbours, &neighbourId, pIt->x, pIt->y);

        if (neighbourId == csIt->currentId || csIt->obsolete)
          neighbourId = -1;

        if (neighbourId > -1)
        {
          outGraph.edges.push_back(EdgeValue(Edge(csIt->currentId, neighbourId), csIt->edgeValue));
        }
        // end of line
        else if (neighbours.empty())
        {
          if (!csIt->obsolete)
            checkEndOfLine(*csIt, lCurrentStage, idCounter, aPointIds, outGraph);

          continue;
        }

        if (!csIt->obsolete)
          checkStagesCollide(*csIt, lCurrentStage, idCounter, aPointIds, outGraph);

        PointValueListGroup lGroups;
        groupPoints(neighbours, lGroups);

        if (lGroups.size() > 1 && csIt->iteration > 0)
        {
          int currentId = idCounter;

          addToResults(idCounter, csIt->currentId, aPointIds, outGraph, csIt->points, csIt->edgeValue);

          csIt->iteration = 0;
          csIt->currentId = neighbourId > -1 ? neighbourId : currentId;
        }

        int currentId = idCounter;
        for (PointValueListGroup::iterator gIt = lGroups.begin(); gIt != lGroups.end(); ++gIt)
        {
          PointListi resultPoints = fromPointValueList(*gIt);

          Stage stage;
          stage.points = resultPoints;
          stage.edgeValue = csIt->edgeValue;
          stage.obsolete = false;

          if (csIt->iteration == depth - 1)
          {
            stage.iteration = 0;
            stage.currentId = neighbourId > -1 ? neighbourId : currentId;

            addToResults(idCounter, csIt->currentId, aPointIds, outGraph, resultPoints, csIt->edgeValue);
          }
          else
          {
            cv::Vec3b color = randomColor();

            for (PointValueList::iterator pIt = gIt->begin(); pIt != gIt->end(); ++pIt)
              display.at<cv::Vec3b>(cv::Point(pIt->first.x, pIt->first.y)) = color;

            stage.iteration = csIt->iteration + 1;
            stage.currentId = neighbourId > -1 ? neighbourId : csIt->currentId;
          }

          for (PointValueList::iterator pIt = gIt->begin(); pIt != gIt->end(); ++pIt)
          {
            if (pIt->second != stage.edgeValue)
            {
              stage.edgeValue = pIt->second;
              break;
            }
          }

          lNextStage.push_back(stage);
        }
      }

      lCurrentStage = lNextStage;

    } while (!lCurrentStage.empty());
  }

  void removeEdge(Graph& graph, int p1, int p2)
  {
    EdgeValueList::iterator it = graph.edges.begin();
    while (it != graph.edges.end())
    {
      if ((p1 == it->first.first && p2 == it->first.second) ||
        (p2 == it->first.first && p1 == it->first.second))
      {
        it = graph.edges.erase(it);
      }
      else
      {
        ++it;
      }
    }
  }

  void removeDeadEndEdges(Graph& graph, NeighbourMap& neighbours, IdSet& ignore, int pointId, NeighbourValueList nlist)
  {
    int currentPoint = pointId;
    int prevPoint = -1;

    while (nlist.size() <= 2)
    {
      for (NeighbourValueList::iterator it = nlist.begin(); it != nlist.end(); ++it)
      {
        if (it->first != prevPoint)
        {
          removeEdge(graph, currentPoint, it->first);
          graph.points.erase(graph.points.find(currentPoint));

          prevPoint = currentPoint;
          currentPoint = it->first;
          nlist = neighbours.find(it->first)->second;
          break;
        }
      }

      if (nlist.size() == 1)
      {
        graph.points.erase(graph.points.find(currentPoint));
        ignore.insert(currentPoint);
        break;
      }
    }

    for (NeighbourValueList::iterator it = nlist.begin(); it != nlist.end(); ++it)
    {
      if (it->first == prevPoint)
      {
        nlist.erase(it);
        neighbours.find(currentPoint)->second = nlist;
        break;
      }
    }
  }

  void removeDeadEnds(Graph& graph)
  {
    ProgressPrinter progress("Remove dead ends");

    NeighbourMap neighbours;
    neighbourMapFromEdges(graph.edges, neighbours);

    IdSet ignorePoints;

    int i = 0;
    for (NeighbourMap::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
    {
      if (it->second.size() == 1 && ignorePoints.find(it->first) == ignorePoints.end())
        removeDeadEndEdges(graph, neighbours, ignorePoints, it->first, it->second);

      i++;
      progress.update(i, neighbours.size());
    }
  }

  void detectLines(cv::Mat& inputImage, cv::Mat& displayImage, int depth, Graph& outGraph)
  {
    ProgressPrinter progress("Detect lines");

    BoolArray aVisited(inputImage.cols, inputImage.rows, false);
    IntArray aPointIds(inputImage.cols, inputImage.rows, -1);
    int idCounter = 0;

    outGraph.boundary = BoundingBox<float>(cv::Point2f(0.0f, 0.0f), cv::Point2f(float(inputImage.cols - 1), float(inputImage.rows - 1)));

    for (int y = 0; y < inputImage.rows; y++)
    {
      for (int x = 0; x < inputImage.cols; x++)
      {
        uchar edgeVal = inputImage.at<uchar>(cv::Point(x, y));

        if (edgeVal > 0 && !aVisited.get(x, y))
        {
          findNeighbours(inputImage, displayImage, aVisited, aPointIds, outGraph, x, y, edgeVal, depth, idCounter);
        }

        progress.update(long(y) * inputImage.cols + x, long(inputImage.rows) * inputImage.cols - 1);
      }
    }
  }
}