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

Vec3f randomColor()
{
  return Vec3b(rand() % 155 + 100, rand() % 155 + 100, rand() % 155 + 100);
}

void checkPixel(Mat& image, BoolArray& aVisited, PointList& points, int x, int y)
{
  if (x >= 0 && y >= 0 && x < image.cols && y < image.rows)
    if (image.at<uchar>(Point(x, y)) == 255)
      if (!aVisited.get(x, y))
        points.push_back(Point(x, y));
}

void checkPixelsAround(Mat& image, BoolArray& aVisited, PointList& points, int x, int y)
{
  checkPixel(image, aVisited, points, x - 1, y - 1);
  checkPixel(image, aVisited, points, x    , y - 1);
  checkPixel(image, aVisited, points, x + 1, y - 1);
  checkPixel(image, aVisited, points, x - 1, y    );
  checkPixel(image, aVisited, points, x + 1, y    );
  checkPixel(image, aVisited, points, x - 1, y + 1);
  checkPixel(image, aVisited, points, x    , y + 1);
  checkPixel(image, aVisited, points, x + 1, y + 1);
}

void findNeighbours(Mat& image, Mat& display, BoolArray& aVisited, PointList& lNeighbours, PointList& lResult, int x, int y, int depth, const Vec3b& displayColor)
{
  if (depth == 0)
    return;

  checkPixelsAround(image, aVisited, lNeighbours, x, y);

  // reached end
  if (lNeighbours.size() == 0)
    return;

  for (PointList::iterator it = lNeighbours.begin(); it != lNeighbours.end(); ++it)
  {
    // color
    if (depth > 1)
      display.at<Vec3b>(Point(it->x, it->y)) = displayColor;

    aVisited.set(it->x, it->y, true);
  }

  PointList neighbours;
  Vec3f color = randomColor();
  for (PointList::iterator it = lNeighbours.begin(); it != lNeighbours.end(); ++it)
    findNeighbours(image, display, aVisited, neighbours, lResult, it->x, it->y, depth - 1, color);

  if (neighbours.size() == 0)
  {
    for (PointList::iterator it = lNeighbours.begin(); it != lNeighbours.end(); ++it)
    lResult.push_back(*it);
  }

  if (depth == 1)
  {
    for (PointList::iterator it = lNeighbours.begin(); it != lNeighbours.end(); ++it)
      lResult.push_back(*it);
  }  
}

void searchPath(Mat& image, Mat& display, BoolArray& aVisited, int x, int y)
{
  PointList lResult;
  PointList lNeighbours;

  aVisited.set(x, y, true);
  findNeighbours(image, display, aVisited, lNeighbours, lResult, x, y, 10, randomColor());

  printf("bla\n");
}

void findEntries(Mat& image, Mat& display)
{
  BoolArray aVisited(image.cols, image.rows, false);

	for (int y = 5; y < image.rows; y++)
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