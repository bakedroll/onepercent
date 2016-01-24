#include "detect.h"

namespace helper
{
  typedef struct _stage
  {
    PointList points;
    int iteration;
    int currentId;
    uchar edgeValue;
    bool obsolete;
  } Stage;

  typedef std::vector<Stage> StageList;

  class BoundingBox
  {
  public:
    BoundingBox(PointList& points)
    {
      m_min = cv::Point(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
      m_max = cv::Point(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());

      for (PointList::iterator it = points.begin(); it != points.end(); ++it)
      {
        m_min.x = std::min(m_min.x, it->x);
        m_min.y = std::min(m_min.y, it->y);
        m_max.x = std::max(m_max.x, it->x);
        m_max.y = std::max(m_max.y, it->y);
      }
    }

    cv::Point min()
    {
      return m_min;
    }

    cv::Point max()
    {
      return m_max;
    }

    int width()
    {
      return m_max.x - m_min.x + 1;
    }

    int height()
    {
      return m_max.y - m_min.y + 1;
    }

  private:
    cv::Point m_min;
    cv::Point m_max;
  };

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

  PointList fromPointValueList(PointValueList& list)
  {
    PointList result;
    for (PointValueList::iterator it = list.begin(); it != list.end(); ++it)
      result.push_back(it->first);

    return result;
  }

  void groupPoints(PointValueList& points, PointValueListGroup& groups)
  {
    PointList pointList = fromPointValueList(points);

    BoundingBox fieldAabb(pointList);
    cv::Mat field(fieldAabb.height(), fieldAabb.width(), CV_8UC1);
    BoolArray visited(fieldAabb.width(), fieldAabb.height(), false);

    field.setTo(0);

    for (PointValueList::iterator it = points.begin(); it != points.end(); ++it)
      field.at<uchar>(it->first - fieldAabb.min()) = it->second;

    for (int y = 0; y < fieldAabb.height(); y++)
    {
      for (int x = 0; x < fieldAabb.width(); x++)
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

  void addToResults(int& idCounter, int connectTo, IntArray& aPointIds, Graph& outGraph, const PointList& points, uchar edgeVal = 255)
  {
    cv::Point2f p(0, 0);

    for (PointList::const_iterator pIt = points.cbegin(); pIt != points.cend(); ++pIt)
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
    for (PointList::iterator sit = stage.points.begin(); sit != stage.points.end(); ++sit)
      for (PointList::iterator nit = next.points.begin(); nit != next.points.end(); ++nit)
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

    PointList points;
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

        for (PointList::iterator pIt = csIt->points.begin(); pIt != csIt->points.end(); ++pIt)
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
          PointList resultPoints = fromPointValueList(*gIt);

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

  void incrementEdgeCount(PointEdgesCountMap& peCountMap, int pointId)
  {
    PointEdgesCountMap::iterator it = peCountMap.find(pointId);

    if (it != peCountMap.end())
    {
      it->second++;
      return;
    }

    peCountMap.insert(PointEdgesCountMap::value_type(pointId, 1));
  }

  void cleanUpDeadEnds(Graph& outGraph)
  {
    typedef std::vector<IdPointMap::iterator> RPMIteratorList;

    PointEdgesCountMap peCountMap;

    for (EdgeValueList::iterator eIt = outGraph.edges.begin(); eIt != outGraph.edges.end(); ++eIt)
    {
      incrementEdgeCount(peCountMap, eIt->first.first);
      incrementEdgeCount(peCountMap, eIt->first.second);
    }

    RPMIteratorList pointsToRemove;
    for (IdPointMap::iterator pIt = outGraph.points.begin(); pIt != outGraph.points.end(); ++pIt)
    {
      PointEdgesCountMap::iterator peIt = peCountMap.find(pIt->first);
      assert(peIt != peCountMap.end());

      if (peIt->second == 1)
      {
        EdgeValueList::iterator edgeToRemoveIt;

        int neighbourId = -1;
        for (EdgeValueList::iterator eIt = outGraph.edges.begin(); eIt != outGraph.edges.end(); ++eIt)
        {
          if (eIt->first.first == pIt->first)
          {
            neighbourId = eIt->first.second;
            edgeToRemoveIt = eIt;
            break;
          }
          if (eIt->first.second == pIt->first)
          {
            neighbourId = eIt->first.first;
            edgeToRemoveIt = eIt;
            break;
          }
        }

        assert(neighbourId > -1);
        PointEdgesCountMap::iterator peIt2 = peCountMap.find(neighbourId);
        assert(peIt2 != peCountMap.end());

        if (peIt2->second > 2)
        {
          pointsToRemove.push_back(pIt);
          outGraph.edges.erase(edgeToRemoveIt);
        }
      }
    }

    for (RPMIteratorList::iterator it = pointsToRemove.begin(); it != pointsToRemove.end(); ++it)
      outGraph.points.erase(*it);
  }

  void findEntries(cv::Mat& image, cv::Mat& result, int depth, Graph& outGraph)
  {
    BoolArray aVisited(image.cols, image.rows, false);
    IntArray aPointIds(image.cols, image.rows, -1);
    int idCounter = 0;

    for (int y = 0; y < image.rows; y++)
    {
      for (int x = 0; x < image.cols; x++)
      {
        uchar edgeVal = image.at<uchar>(cv::Point(x, y));

        if (edgeVal > 0 && !aVisited.get(x, y))
        {
          findNeighbours(image, result, aVisited, aPointIds, outGraph, x, y, edgeVal, depth, idCounter);
        }
      }
    }
  }

  void detectLines(cv::Mat& inputImage, cv::Mat& displayImage, int depth, Graph& outGraph)
  {
    findEntries(inputImage, displayImage, depth, outGraph);
    cleanUpDeadEnds(outGraph);
  }
}