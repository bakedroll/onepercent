#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int detectLines(const char* in, float display)
{
	typedef vector<Vec4f> LinesList;

	Mat image;
	image = imread(in, IMREAD_GRAYSCALE);

	if (!image.data)
	{
		printf("Error: Image could not be loaded: %s\n", in);
		return -1;
	}

	Mat displayImage(int(image.rows * display), int(image.cols * display), image.type());

	LinesList lines;
	LinesList displayLines;

	//Canny(image, image, 50, 200, 3);

	Ptr<LineSegmentDetector> lsd = createLineSegmentDetector(LSD_REFINE_STD, 0.2);
	lsd->detect(image, lines);

	for (LinesList::iterator it = displayLines.begin(); it != displayLines.end(); ++it)
		for (int i = 0; i < 4;i++)
			(*it).val[i] *= display;
		
	lsd->drawSegments(image, lines);

	namedWindow("Lines", WINDOW_AUTOSIZE);
	imshow("Lines", image);

	return 0;
}

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		cout << "Error: Not enough parameters." << endl;
		return -1;
	}

	int result;
	switch (atoi(argv[1]))
	{
	case 0:
		result = detectLines(argv[2], 0.125f);
		break;

	default:
		cout << "Error: Wrong mode." << endl;
		result = -1;
		break;

	}

	waitKey(0);
	return result;
}