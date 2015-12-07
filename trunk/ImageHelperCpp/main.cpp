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
  PointListGroup lCurrentStage;

  PointList points;
  points.push_back(Point(x, y));

  lCurrentStage.push_back(points);
  aVisited.set(x, y, true);

  int i = 0;
  do
  {
    PointListGroup lNextStage;

    for (PointListGroup::iterator csIt = lCurrentStage.begin(); csIt != lCurrentStage.end(); ++csIt)
    {
      PointList neighbours;

      for (PointList::iterator pIt = csIt->begin(); pIt != csIt->end(); ++pIt)
        checkPixelsAround(image, aVisited, neighbours, pIt->x, pIt->y);

      // end of line
      if (neighbours.empty())
      {
        lResult.push_back(*csIt);

        // color
        for (PointList::iterator pIt = csIt->begin(); pIt != csIt->end(); ++pIt)
          display.at<Vec3b>(Point(pIt->x, pIt->y)) = Vec3b(0, 0, 100);

        //imshow("Lines", display);
        continue;
      }

      PointListGroup lGroups;
      groupPoints(neighbours, lGroups);

      for (PointListGroup::iterator gIt = lGroups.begin(); gIt != lGroups.end(); ++gIt)
        lNextStage.push_back(*gIt);
    }

    // color
    if (i < depth - 1)
    {
      for (PointListGroup::iterator nsIt = lNextStage.begin(); nsIt != lNextStage.end(); ++nsIt)
      {
        Vec3b color = randomColor();

        for (PointList::iterator pIt = nsIt->begin(); pIt != nsIt->end(); ++pIt)
          display.at<Vec3b>(Point(pIt->x, pIt->y)) = color;
      }
    }

    lCurrentStage = lNextStage;
    i++;

    if (i == depth)
    {
      for (PointListGroup::iterator csIt = lCurrentStage.begin(); csIt != lCurrentStage.end(); ++csIt)
      {
        lResult.push_back(*csIt);

        // color
        for (PointList::iterator pIt = csIt->begin(); pIt != csIt->end(); ++pIt)
          display.at<Vec3b>(Point(pIt->x, pIt->y)) = Vec3b(0, 0, 100);

        //imshow("Lines", display);
      }

      i = 0;
    }

  } while (!lCurrentStage.empty());
}

void searchPath(Mat& image, Mat& display, BoolArray& aVisited, int x, int y)
{
  PointListGroup lResult;

  findNeighbours(image, display, aVisited, lResult, x, y, 10);

  printf("bla\n");
}

void findEntries(Mat& image, Mat& display)
{
  BoolArray aVisited(image.cols, image.rows, false);

	for (int y = 0; y < image.rows; y++)
		for (int x = 0; x < image.cols; x++)
      if (image.at<uchar>(Point(x, y)) == 255
        && !aVisited.get(x, y))
      {
        searchPath(image, display, aVisited, x, y);
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

	threshold(image, image, thres, 255, THRESH_BINARY);
  cvtColor(image, displayImage, CV_GRAY2RGB);

	findEntries(image, displayImage);

	namedWindow("Lines", WINDOW_AUTOSIZE);
  imshow("Lines", displayImage);
  imwrite("img.png", displayImage);

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