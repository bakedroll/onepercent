#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

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

typedef std::vector<Point> PointList;
typedef std::vector<PointList> PointListGroup;

typedef struct _stage
{
  PointList points;
  int iteration;
} Stage;

typedef std::vector<Stage> StageList;

class Aabb
{
public:
  Aabb(PointList& points)
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

void checkPixel(Mat& image, BoolArray& aVisited, PointList& points, int x, int y, bool repeat)
{
  if (x < 0) if (repeat) x += image.cols; else return;
  if (y < 0) if (repeat) y += image.rows; else return;
  if (x >= image.cols) if (repeat) x -= image.cols; else return;
  if (y >= image.rows) if (repeat) y -= image.rows; else return;

  if (image.at<uchar>(Point(x, y)) == 255 && !aVisited.get(x, y))
  {
    points.push_back(Point(x, y));
    aVisited.set(x, y, true);
  }
}

void checkPixelsAround(Mat& image, BoolArray& aVisited, PointList& points, int x, int y, bool repeat = true)
{
  checkPixel(image, aVisited, points, x - 1, y - 1, repeat);
  checkPixel(image, aVisited, points, x    , y - 1, repeat);
  checkPixel(image, aVisited, points, x + 1, y - 1, repeat);
  checkPixel(image, aVisited, points, x - 1, y    , repeat);
  checkPixel(image, aVisited, points, x + 1, y    , repeat);
  checkPixel(image, aVisited, points, x - 1, y + 1, repeat);
  checkPixel(image, aVisited, points, x    , y + 1, repeat);
  checkPixel(image, aVisited, points, x + 1, y + 1, repeat);

  /*if (points.size() == 1)
  {
    if (points[0].x != x && points[0].y != y)
      aVisited.set(points[0].x, points[0].y, false);
  }*/
}

void groupPoints(PointList& points, PointListGroup& groups)
{
  Aabb fieldAabb(points);
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
            checkPixelsAround(field, visited, result, it->x, it->y, false);

          for (PointList::iterator it = result.begin(); it != result.end(); ++it)
            group.push_back(*it + fieldAabb.min());

          check = result;
        } while (check.size() > 0);

        groups.push_back(group);
      }
    }
  }
}

void findNeighbours(Mat& image, Mat& display, BoolArray& aVisited, PointListGroup& lResult, int x, int y, int depth)
{
  StageList lCurrentStage;

  PointList points;
  points.push_back(Point(x, y));

  Stage firstStage;
  firstStage.points = points;
  firstStage.iteration = 0;

  lResult.push_back(points);

  lCurrentStage.push_back(firstStage);
  aVisited.set(x, y, true);

  //bool justSplit = false;
  do
  {
    StageList lNextStage;

    for (StageList::iterator csIt = lCurrentStage.begin(); csIt != lCurrentStage.end(); ++csIt)
    {
      PointList neighbours;

      for (PointList::iterator pIt = csIt->points.begin(); pIt != csIt->points.end(); ++pIt)
        checkPixelsAround(image, aVisited, neighbours, pIt->x, pIt->y);

      // end of line
      if (neighbours.empty())
      {
        //if (!justSplit)
        lResult.push_back(csIt->points);

        continue;
      }

      PointListGroup lGroups;
      groupPoints(neighbours, lGroups);

      if (lGroups.size() > 1)
      {
        if (csIt->iteration > 0)
        {
          lResult.push_back(csIt->points);
          csIt->iteration = 0;
        }

        //justSplit = true;
      }
      //else
      //{
      //  justSplit = false;
      //}

      for (PointListGroup::iterator gIt = lGroups.begin(); gIt != lGroups.end(); ++gIt)
      {
        Stage stage;
        stage.points = *gIt;

        if (csIt->iteration == depth-1)
        {
          lResult.push_back(*gIt);
          stage.iteration = 0;
        }
        else
        {
          Vec3b color = randomColor();

          for (PointList::iterator pIt = gIt->begin(); pIt != gIt->end(); ++pIt)
            display.at<Vec3b>(Point(pIt->x, pIt->y)) = color;

          stage.iteration = csIt->iteration + 1;
        }

        lNextStage.push_back(stage);
      }
    }

    lCurrentStage = lNextStage;

  } while (!lCurrentStage.empty());
}

void searchPath(Mat& image, Mat& display, Mat& result, BoolArray& aVisited, int x, int y)
{
  PointListGroup lResult;

  findNeighbours(image, display, aVisited, lResult, x, y, 10);

  for (PointListGroup::iterator rIt = lResult.begin(); rIt != lResult.end(); ++rIt)
  {
    Point p(0, 0);

    for (PointList::iterator pIt = rIt->begin(); pIt != rIt->end(); ++pIt)
    {
      display.at<Vec3b>(Point(pIt->x, pIt->y)) = Vec3b(0, 0, 100);
      p += *pIt;
    }

    p.x /= rIt->size();
    p.y /= rIt->size();

    result.at<Vec3b>(p) = Vec3b(255, 0, 0);
  }



  printf("bla\n");
}

void findEntries(Mat& image, Mat& display, Mat& result)
{
  BoolArray aVisited(image.cols, image.rows, false);

  result.setTo(cv::Scalar(0, 0, 0));

	for (int y = 0; y < image.rows; y++)
		for (int x = 0; x < image.cols; x++)
      if (image.at<uchar>(Point(x, y)) == 255
        && !aVisited.get(x, y))
      {
        searchPath(image, display, result, aVisited, x, y);
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

	findEntries(image, displayImage, resultImage);

	namedWindow("Lines", WINDOW_AUTOSIZE);
  imshow("Lines", displayImage);
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
		result = detectLines(argv[2], 1.0, atoi(argv[3]));//0.125f);
		break;

	default:
		cout << "Error: Wrong mode." << endl;
		result = -1;
		break;

	}

	waitKey(0);
	return result;
}