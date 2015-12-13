#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <map>

using namespace cv;
using namespace std;

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

typedef vector<Point> PointList;
typedef vector<PointList> PointListGroup;
typedef pair<int, int> ResultEdge;
typedef map<int, Point2f> ResultPointMap;
typedef vector<ResultEdge> ResultEdgeMap;
typedef std::map<int, int> PointEdgesCountMap;

typedef struct _stage
{
  PointList points;
  int iteration;
  int currentId;
  //bool justSplit;
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

void checkPixel(Mat& image, BoolArray& aVisited, IntArray* aPointIds, PointList& points, int* neighbourId, int x, int y, bool repeat)
{
  if (x < 0) if (repeat) x += image.cols; else return;
  if (y < 0) if (repeat) y += image.rows; else return;
  if (x >= image.cols) if (repeat) x -= image.cols; else return;
  if (y >= image.rows) if (repeat) y -= image.rows; else return;

  if (image.at<uchar>(Point(x, y)) == 255)
  {
    if (!aVisited.get(x, y))
    {
      points.push_back(Point(x, y));
      aVisited.set(x, y, true);
    }
    else if (aPointIds != nullptr && aPointIds->get(x, y) > -1)
    {
      *neighbourId = aPointIds->get(x, y);
    }
  }
}

void checkPixelsAround(Mat& image, BoolArray& aVisited, IntArray* aPointIds, PointList& points, int* neighbourId, int x, int y, bool repeat = true)
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

void groupPoints(PointList& points, PointListGroup& groups)
{
  BoundingBox fieldAabb(points);
  Mat field(fieldAabb.height(), fieldAabb.width(), CV_8UC1);
  BoolArray visited(fieldAabb.width(), fieldAabb.height(), false);

  field.setTo(0);

  for (PointList::iterator it = points.begin(); it != points.end(); ++it)
    field.at<uchar>(*it - fieldAabb.min()) = 255;

  for (int y = 0; y < fieldAabb.height(); y++)
  {
    for (int x = 0; x < fieldAabb.width(); x++)
    {
      if (field.at<uchar>(Point(x, y)) == 255 && !visited.get(x, y))
      {
        PointList group;

        PointList check;
        check.push_back(Point(x, y));
        group.push_back(Point(x, y) + fieldAabb.min());

        visited.set(x, y, true);

        do
        {
          PointList result;
          for (PointList::iterator it = check.begin(); it != check.end(); ++it)
            checkPixelsAround(field, visited, nullptr, result, nullptr, it->x, it->y, false);

          for (PointList::iterator it = result.begin(); it != result.end(); ++it)
            group.push_back(*it + fieldAabb.min());

          check = result;
        } while (check.size() > 0);

        groups.push_back(group);
      }
    }
  }
}

void addToResults(int& idCounter, int connectTo, IntArray& aPointIds, ResultPointMap& mResultPoints, ResultEdgeMap& mResultEdges, const PointList& points)
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
    mResultEdges.push_back(ResultEdge(connectTo, idCounter));

  idCounter++;
}

void findNeighbours(Mat& image, Mat& display, BoolArray& aVisited, IntArray& aPointIds, ResultPointMap& mResults, ResultEdgeMap& mEdges, int x, int y, int depth)
{
  int idCounter = 0;

  StageList lCurrentStage;

  PointList points;
  points.push_back(Point(x, y));

  Stage firstStage;
  firstStage.points = points;
  firstStage.iteration = 0;
  firstStage.currentId = idCounter;
  //firstStage.justSplit = false;

  addToResults(idCounter, -1, aPointIds, mResults, mEdges, points);

  lCurrentStage.push_back(firstStage);
  aVisited.set(x, y, true);

  do
  {
    StageList lNextStage;

    for (StageList::iterator csIt = lCurrentStage.begin(); csIt != lCurrentStage.end(); ++csIt)
    {
      PointList neighbours;
      int neighbourId;

      for (PointList::iterator pIt = csIt->points.begin(); pIt != csIt->points.end(); ++pIt)
        checkPixelsAround(image, aVisited, &aPointIds, neighbours, &neighbourId, pIt->x, pIt->y);

      if (neighbourId == csIt->currentId)
        neighbourId = -1;

      if (neighbourId > -1)
      {
        mEdges.push_back(ResultEdge(csIt->currentId, neighbourId));
      }
      // end of line
      else if (neighbours.empty())
      {
        //if (!csIt->justSplit)
          addToResults(idCounter, csIt->currentId, aPointIds, mResults, mEdges, csIt->points);

        continue;
      }

      PointListGroup lGroups;
      groupPoints(neighbours, lGroups);

      if (lGroups.size() > 1 && csIt->iteration > 0)
      {
        int currentId = idCounter;

        addToResults(idCounter, csIt->currentId, aPointIds, mResults, mEdges, csIt->points);

        csIt->iteration = 0;
        csIt->currentId = neighbourId > -1 ? neighbourId : currentId;
        //csIt->justSplit = true;
      }
      else
      {
        //csIt->justSplit = false;
      }

      int currentId = idCounter;
      for (PointListGroup::iterator gIt = lGroups.begin(); gIt != lGroups.end(); ++gIt)
      {
        Stage stage;
        stage.points = *gIt;
        //stage.justSplit = csIt->justSplit;

        if (csIt->iteration == depth-1)
        {
          stage.iteration = 0;
          stage.currentId = neighbourId > -1 ? neighbourId : currentId;

          addToResults(idCounter, csIt->currentId, aPointIds, mResults, mEdges, *gIt);
        }
        else
        {
          Vec3b color = randomColor();

          for (PointList::iterator pIt = gIt->begin(); pIt != gIt->end(); ++pIt)
            display.at<Vec3b>(Point(pIt->x, pIt->y)) = color;

          stage.iteration = csIt->iteration + 1;
          stage.currentId = neighbourId > -1 ? neighbourId : csIt->currentId;
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

void cleanUpDeadEnds(ResultPointMap& mResults, ResultEdgeMap& mEdges)
{
  typedef vector<ResultPointMap::iterator> RPMIteratorList;

  PointEdgesCountMap peCountMap;

  for (ResultEdgeMap::iterator eIt = mEdges.begin(); eIt != mEdges.end(); ++eIt)
  {
    incrementEdgeCount(peCountMap, eIt->first);
    incrementEdgeCount(peCountMap, eIt->second);
  }

  RPMIteratorList pointsToRemove;
  for (ResultPointMap::iterator pIt = mResults.begin(); pIt != mResults.end(); ++pIt)
  {
    PointEdgesCountMap::iterator peIt = peCountMap.find(pIt->first);
    assert(peIt != peCountMap.end());

    if (peIt->second == 1)
    {
      ResultEdgeMap::iterator edgeToRemoveIt;

      int neighbourId = -1;
      for (ResultEdgeMap::iterator eIt = mEdges.begin(); eIt != mEdges.end(); ++eIt)
      {
        if (eIt->first == pIt->first)
        {
          neighbourId = eIt->second;
          edgeToRemoveIt = eIt;
          break;
        }
        if (eIt->second == pIt->first)
        {
          neighbourId = eIt->first;
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

void searchPath(Mat& image, Mat& display, Mat& result, BoolArray& aVisited, IntArray& aPointIds, int x, int y, int displaySize)
{
  ResultPointMap mResults;
  ResultEdgeMap mEdges;

  findNeighbours(image, display, aVisited, aPointIds, mResults, mEdges, x, y, 10);
  cleanUpDeadEnds(mResults, mEdges);

  for (ResultEdgeMap::iterator eIt = mEdges.begin(); eIt != mEdges.end(); ++eIt)
    line(result, mResults.find(eIt->first)->second * displaySize, mResults.find(eIt->second)->second * displaySize, Scalar(0, 0, 255));

  for (ResultPointMap::iterator rIt = mResults.begin(); rIt != mResults.end(); ++rIt)
    result.at<Vec3b>(rIt->second * displaySize) = Vec3b(255, 0, 0);

  printf("bla\n");
}

void findEntries(Mat& image, Mat& display, Mat& result, int displaySize)
{
  BoolArray aVisited(image.cols, image.rows, false);
  IntArray aPointIds(image.cols, image.rows, -1);

  result.setTo(cv::Scalar(0, 0, 0));

	for (int y = 0; y < image.rows; y++)
		for (int x = 0; x < image.cols; x++)
      if (image.at<uchar>(Point(x, y)) == 255
        && !aVisited.get(x, y))
      {
        searchPath(image, display, result, aVisited, aPointIds, x, y, displaySize);
        return;
      }
}

int detectLines(const char* in, float display, int thres)
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

	threshold(image, image, thres, 255, THRESH_BINARY);
  cvtColor(image, displayImage, CV_GRAY2RGB);

	findEntries(image, displayImage, resultImage, int(display));

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
		result = detectLines(argv[2], 2.0, atoi(argv[3]));//0.125f);
		break;

	default:
		cout << "Error: Wrong mode." << endl;
		result = -1;
		break;

	}

	waitKey(0);
	return result;
}