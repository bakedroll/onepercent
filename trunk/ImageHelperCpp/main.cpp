#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <map>

using namespace cv;
using namespace std;

#define PLACEHOLDER 50

template<typename T>
class Array
{
public:
  Array(int cols, int rows, T initialValue)
    : m_cols(cols)
    , m_rows(rows)
	{
    m_data = new T*[m_cols];
    for (int x = 0; x < m_cols; x++)
    {
      m_data[x] = new T[m_rows];
      
      for (int y = 0; y < m_rows; y++)
      {
        m_data[x][y] = initialValue;
      }
    }
	}

  ~Array()
	{
    for (int x = 0; x < m_cols; x++)
      delete [] m_data[x];
    delete [] m_data;
	}

  int cols()
	{
    return m_cols;
	}

  int rows()
	{
    return m_rows;
	}

  T get(int col, int row)
	{
    return m_data[col][row];
	}

  void set(int col, int row, T value)
	{
    m_data[col][row] = value;
	}

private:
	T** m_data;
  int m_cols;
  int m_rows;
};

typedef Array<bool> BoolArray;
typedef Array<int> IntArray;
typedef pair<Point, uchar> PointValue;

typedef vector<Point> PointList;
typedef vector<PointValue> PointValueList;
typedef vector<PointValueList> PointValueListGroup;
typedef pair<int, int> ResultEdge;
typedef pair<ResultEdge, uchar> ResultEdgeValue;
typedef map<int, Point2f> ResultPointMap;
typedef vector<ResultEdgeValue> ResultEdgeValueList;
typedef map<int, int> PointEdgesCountMap;

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
    m_min = Point(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    m_max = Point(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());

    for (PointList::iterator it = points.begin(); it != points.end(); ++it)
    {
      m_min.x = std::min(m_min.x, it->x);
      m_min.y = std::min(m_min.y, it->y);
      m_max.x = std::max(m_max.x, it->x);
      m_max.y = std::max(m_max.y, it->y);
    }
  }

  Point min()
  {
    return m_min;
  }

  Point max()
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
  Point m_min;
  Point m_max;
};

Vec3f randomColor()
{
  return Vec3b(rand() % 100 + 155, rand() % 100 + 155, rand() % 100 + 155);
}

void writePolyFile(ResultPointMap& points, ResultEdgeValueList& edges, char* filename)
{
  ofstream fs(filename);
  if (!fs.is_open())
    return;

  fs << int(points.size()) << " 2 0 0" << endl;

  for (ResultPointMap::iterator it = points.begin(); it != points.end(); ++it)
    fs << it->first << " " << it->second.x << " " << it->second.y << endl;

  fs << int(edges.size()) << " 1" << endl;

  int i = 0;
  for (ResultEdgeValueList::iterator it = edges.begin(); it != edges.end(); ++it)
  {
    fs << i << " " << it->first.first << " " << it->first.second << " 1" << endl;
    i++;
  }

  fs.close();
}

void checkPixel(Mat& image, BoolArray& aVisited, IntArray* aPointIds, PointValueList& points, int* neighbourId, int x, int y, bool repeat)
{
  if (x < 0) if (repeat) x += image.cols; else return;
  if (y < 0) if (repeat) y += image.rows; else return;
  if (x >= image.cols) if (repeat) x -= image.cols; else return;
  if (y >= image.rows) if (repeat) y -= image.rows; else return;

  uchar pval = image.at<uchar>(Point(x, y));
  if (pval > 0)
  {
    if (!aVisited.get(x, y))
    {
      points.push_back(PointValue(Point(x, y), pval));
      aVisited.set(x, y, true);
    }
    else if (aPointIds != nullptr && aPointIds->get(x, y) > -1)
    {
      *neighbourId = aPointIds->get(x, y);
    }
  }
}

void checkPixelsAround(Mat& image, BoolArray& aVisited, IntArray* aPointIds, PointValueList& points, int* neighbourId, int x, int y, bool repeat = true)
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
  Mat field(fieldAabb.height(), fieldAabb.width(), CV_8UC1);
  BoolArray visited(fieldAabb.width(), fieldAabb.height(), false);

  field.setTo(0);

  for (PointValueList::iterator it = points.begin(); it != points.end(); ++it)
    field.at<uchar>(it->first - fieldAabb.min()) = it->second;

  for (int y = 0; y < fieldAabb.height(); y++)
  {
    for (int x = 0; x < fieldAabb.width(); x++)
    {
      uchar pval = field.at<uchar>(Point(x, y));
      if (pval > 0 && !visited.get(x, y))
      {
        PointValueList group;

        PointValueList check;
        check.push_back(PointValue(Point(x, y), pval));
        group.push_back(PointValue(Point(x, y) + fieldAabb.min(), pval));

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
  Point2f p(0, 0);

  for (PointList::const_iterator pIt = points.cbegin(); pIt != points.cend(); ++pIt)
  {
    aPointIds.set(pIt->x, pIt->y, idCounter);
    p += Point2f(float(pIt->x), float(pIt->y));
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

void checkEndOfLine(Stage& stage, StageList& nextStage, int& idCounter, IntArray& aPointIds, ResultPointMap& mResults, ResultEdgeValueList& mEdges)
{
  addToResults(idCounter, stage.currentId, aPointIds, mResults, mEdges, stage.points, stage.edgeValue);

  for (StageList::iterator it = nextStage.begin(); it != nextStage.end(); ++it)
  {
    if (&*it != &stage && stagesCollide(stage, *it))
    {
      mEdges.push_back(ResultEdgeValue(ResultEdge(idCounter - 1, it->currentId), it->edgeValue));
      it->obsolete = true;
      return;
    }
  }
}

void findNeighbours(Mat& image, Mat& display, BoolArray& aVisited, IntArray& aPointIds, ResultPointMap& mResults, ResultEdgeValueList& mEdges, int x, int y, uchar edgeVal, int depth)
{
  int idCounter = 0;

  StageList lCurrentStage;

  PointList points;
  points.push_back(Point(x, y));

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
      if (csIt->obsolete)
        continue;

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
        checkEndOfLine(*csIt, lCurrentStage, idCounter, aPointIds, mResults, mEdges);
        continue;
      }

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

        if (csIt->iteration == depth-1)
        {
          stage.iteration = 0;
          stage.currentId = neighbourId > -1 ? neighbourId : currentId;

          addToResults(idCounter, csIt->currentId, aPointIds, mResults, mEdges, resultPoints, csIt->edgeValue);
        }
        else
        {
          Vec3b color = randomColor();

          for (PointValueList::iterator pIt = gIt->begin(); pIt != gIt->end(); ++pIt)
            display.at<Vec3b>(Point(pIt->first.x, pIt->first.y)) = color;

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
  typedef vector<ResultPointMap::iterator> RPMIteratorList;

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

void searchPath(Mat& image, Mat& display, Mat& result, BoolArray& aVisited, IntArray& aPointIds, int x, int y, uchar edgeVal, int displaySize, int depth, ResultPointMap& mResultPoints, ResultEdgeValueList& mResultEdges)
{
  findNeighbours(image, display, aVisited, aPointIds, mResultPoints, mResultEdges, x, y, edgeVal, depth);
  cleanUpDeadEnds(mResultPoints, mResultEdges);
}

void findEntries(Mat& image, Mat& display, Mat& result, int displaySize, int depth, char* outPoly)
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
      uchar edgeVal = image.at<uchar>(Point(x, y));

      if (edgeVal > 0 && !aVisited.get(x, y))
      {
        searchPath(image, display, result, aVisited, aPointIds, x, y, edgeVal, displaySize, depth, mResultPoints, mResultEdges);
      }
    }
  }

  for (ResultEdgeValueList::iterator eIt = mResultEdges.begin(); eIt != mResultEdges.end(); ++eIt)
    line(result, mResultPoints.find(eIt->first.first)->second * displaySize, mResultPoints.find(eIt->first.second)->second * displaySize, Scalar(0, 0, eIt->second));

  for (ResultPointMap::iterator rIt = mResultPoints.begin(); rIt != mResultPoints.end(); ++rIt)
    result.at<Vec3b>(rIt->second * displaySize) = Vec3b(255, 0, 0);

  writePolyFile(mResultPoints, mResultEdges, outPoly);
}

int detectLines(const char* in, float display, int depth, char* outPoly)
{
	typedef vector<Vec4f> LinesList;

	Mat image;

	image = imread(in, IMREAD_GRAYSCALE);

	if (!image.data)
	{
		printf("Error: Image could not be loaded: %s\n", in);
		return -1;
	}

	Mat displayImage(int(image.rows * display), int(image.cols * display), CV_8UC3);
  Mat resultImage(int(image.rows * display), int(image.cols * display), CV_8UC3);

  cvtColor(image, displayImage, CV_GRAY2RGB);

	findEntries(image, displayImage, resultImage, int(display), depth, outPoly);

	namedWindow("Lines", WINDOW_AUTOSIZE);
  imshow("Lines", displayImage);
  imshow("Result", resultImage);
  imwrite("img.png", displayImage);
  imwrite("result.png", resultImage);

	return 0;
}

int main(int argc, char** argv)
{
	if (argc < 4)
	{
		cout << "Error: Not enough parameters." << endl;
		return -1;
	}

	int result;
	switch (atoi(argv[1]))
	{
	case 0:
    result = detectLines(argv[2], float(atof(argv[4])), atoi(argv[3]), argv[5]);
		break;

	default:
		cout << "Error: Wrong mode." << endl;
		result = -1;
		break;

	}

	waitKey(0);
	return result;
}