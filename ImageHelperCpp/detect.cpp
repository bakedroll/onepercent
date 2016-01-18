#include "detect.h"

#include "types.h"
#include "reduce.h"
#include "io.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

    /*if (points.size() == 1)
    {
    if (points[0].x != x && points[0].y != y)
    aVisited.set(points[0].x, points[0].y, false);
    }*/
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

  void addToResults(int& idCounter, int connectTo, IntArray& aPointIds, ResultPointMap& mResultPoints, ResultEdgeValueList& mResultEdges, const PointList& points, uchar edgeVal = 255)
  {
    cv::Point2f p(0, 0);

    for (PointList::const_iterator pIt = points.cbegin(); pIt != points.cend(); ++pIt)
    {
      aPointIds.set(pIt->x, pIt->y, idCounter);
      p += cv::Point2f(float(pIt->x), float(pIt->y));
    }

    p.x /= float(points.size());
    p.y /= float(points.size());

    mResultPoints.insert(ResultPointMap::value_type(idCounter, p));

    if (connectTo > -1)
      mResultEdges.push_back(ResultEdgeValue(ResultEdge(connectTo, idCounter), edgeVal));

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

  bool checkStagesCollide(Stage& stage, StageList& nextStage, int& idCounter, IntArray& aPointIds, ResultPointMap& mResults, ResultEdgeValueList& mEdges)
  {
    for (StageList::iterator it = nextStage.begin(); it != nextStage.end(); ++it)
    {
      if ((&*it != &stage) /*&& (idCounter-1 != it->currentId)*/ && stagesCollide(stage, *it))
      {
        addToResults(idCounter, stage.currentId, aPointIds, mResults, mEdges, stage.points, stage.edgeValue);
        mEdges.push_back(ResultEdgeValue(ResultEdge(idCounter - 1, it->currentId), it->edgeValue));

        it->obsolete = true;
        stage.iteration = 0;
        stage.currentId = idCounter - 1;

        return true;
      }
    }

    return false;
  }

  void checkEndOfLine(Stage& stage, StageList& nextStage, int& idCounter, IntArray& aPointIds, ResultPointMap& mResults, ResultEdgeValueList& mEdges)
  {
    if (!checkStagesCollide(stage, nextStage, idCounter, aPointIds, mResults, mEdges))
      addToResults(idCounter, stage.currentId, aPointIds, mResults, mEdges, stage.points, stage.edgeValue);
  }

  void findNeighbours(cv::Mat& image, cv::Mat& display, BoolArray& aVisited, IntArray& aPointIds, ResultPointMap& mResults, ResultEdgeValueList& mEdges, int x, int y, uchar edgeVal, int depth)
  {
    int idCounter = 0;

    StageList lCurrentStage;

    PointList points;
    points.push_back(cv::Point(x, y));

    Stage firstStage;
    firstStage.points = points;
    firstStage.iteration = 0;
    firstStage.currentId = idCounter;
    firstStage.edgeValue = edgeVal;
    firstStage.obsolete = false;

    addToResults(idCounter, -1, aPointIds, mResults, mEdges, points);

    lCurrentStage.push_back(firstStage);
    aVisited.set(x, y, true);

    do
    {
      StageList lNextStage;

      for (StageList::iterator csIt = lCurrentStage.begin(); csIt != lCurrentStage.end(); ++csIt)
      {
        //if (csIt->obsolete)
        //  continue;

        PointValueList neighbours;
        int neighbourId;

        for (PointList::iterator pIt = csIt->points.begin(); pIt != csIt->points.end(); ++pIt)
          checkPixelsAround(image, aVisited, &aPointIds, neighbours, &neighbourId, pIt->x, pIt->y);

        if (neighbourId == csIt->currentId)
          neighbourId = -1;

        if (neighbourId > -1)
        {
          mEdges.push_back(ResultEdgeValue(ResultEdge(csIt->currentId, neighbourId), csIt->edgeValue));
        }
        // end of line
        else if (neighbours.empty())
        {
          if (!csIt->obsolete)
            checkEndOfLine(*csIt, lCurrentStage, idCounter, aPointIds, mResults, mEdges);

          continue;
        }

        if (!csIt->obsolete)
          checkStagesCollide(*csIt, lCurrentStage, idCounter, aPointIds, mResults, mEdges);

        PointValueListGroup lGroups;
        groupPoints(neighbours, lGroups);

        if (lGroups.size() > 1 && csIt->iteration > 0)
        {
          int currentId = idCounter;

          addToResults(idCounter, csIt->currentId, aPointIds, mResults, mEdges, csIt->points, csIt->edgeValue);

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

            addToResults(idCounter, csIt->currentId, aPointIds, mResults, mEdges, resultPoints, csIt->edgeValue);
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

  void cleanUpDeadEnds(ResultPointMap& mResults, ResultEdgeValueList& mEdges)
  {
    typedef std::vector<ResultPointMap::iterator> RPMIteratorList;

    PointEdgesCountMap peCountMap;

    for (ResultEdgeValueList::iterator eIt = mEdges.begin(); eIt != mEdges.end(); ++eIt)
    {
      incrementEdgeCount(peCountMap, eIt->first.first);
      incrementEdgeCount(peCountMap, eIt->first.second);
    }

    RPMIteratorList pointsToRemove;
    for (ResultPointMap::iterator pIt = mResults.begin(); pIt != mResults.end(); ++pIt)
    {
      PointEdgesCountMap::iterator peIt = peCountMap.find(pIt->first);
      assert(peIt != peCountMap.end());

      if (peIt->second == 1)
      {
        ResultEdgeValueList::iterator edgeToRemoveIt;

        int neighbourId = -1;
        for (ResultEdgeValueList::iterator eIt = mEdges.begin(); eIt != mEdges.end(); ++eIt)
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
          mEdges.erase(edgeToRemoveIt);
        }
      }
    }

    for (RPMIteratorList::iterator it = pointsToRemove.begin(); it != pointsToRemove.end(); ++it)
      mResults.erase(*it);
  }

  void searchPath(cv::Mat& image, cv::Mat& display, cv::Mat& result, BoolArray& aVisited, IntArray& aPointIds, int x, int y, uchar edgeVal, int depth, ResultPointMap& mResultPoints, ResultEdgeValueList& mResultEdges)
  {
    findNeighbours(image, display, aVisited, aPointIds, mResultPoints, mResultEdges, x, y, edgeVal, depth);
    cleanUpDeadEnds(mResultPoints, mResultEdges);
  }

  void findEntries(cv::Mat& image, cv::Mat& display, cv::Mat& result, int displaySize, int depth, char* outPoly)
  {
    ResultPointMap mResultPoints;
    ResultEdgeValueList mResultEdges;

    BoolArray aVisited(image.cols, image.rows, false);
    IntArray aPointIds(image.cols, image.rows, -1);

    result.setTo(cv::Scalar(0, 0, 0));

    for (int y = 0; y < image.rows; y++)
    {
      for (int x = 0; x < image.cols; x++)
      {
        uchar edgeVal = image.at<uchar>(cv::Point(x, y));

        if (edgeVal > 0 && !aVisited.get(x, y))
        {
          searchPath(image, display, result, aVisited, aPointIds, x, y, edgeVal, depth, mResultPoints, mResultEdges);
        }
      }
    }

    for (ResultEdgeValueList::iterator eIt = mResultEdges.begin(); eIt != mResultEdges.end(); ++eIt)
      line(result, mResultPoints.find(eIt->first.first)->second * displaySize, mResultPoints.find(eIt->first.second)->second * displaySize, cv::Scalar(0, 0, eIt->second));

    for (ResultPointMap::iterator rIt = mResultPoints.begin(); rIt != mResultPoints.end(); ++rIt)
      result.at<cv::Vec3b>(rIt->second * displaySize) = cv::Vec3b(255, 0, 0);

    reducePoints(mResultPoints, mResultEdges);

    writePolyFile(mResultPoints, mResultEdges, outPoly);
  }

  int detectLines(const char* in, float display, int depth, char* outPoly)
  {
    typedef std::vector<cv::Vec4f> LinesList;

    cv::Mat image;

    image = cv::imread(in, cv::IMREAD_GRAYSCALE);

    if (!image.data)
    {
      printf("Error: Image could not be loaded: %s\n", in);
      return -1;
    }

    cv::Mat displayImage(int(image.rows * display), int(image.cols * display), CV_8UC3);
    cv::Mat resultImage(int(image.rows * display), int(image.cols * display), CV_8UC3);

    cvtColor(image, displayImage, CV_GRAY2RGB);

    findEntries(image, displayImage, resultImage, int(display), depth, outPoly);

    cv::namedWindow("Lines", cv::WINDOW_AUTOSIZE);
    imshow("Lines", displayImage);
    imshow("Result", resultImage);
    imwrite("img.png", displayImage);
    imwrite("result.png", resultImage);

    return 0;
  }
}