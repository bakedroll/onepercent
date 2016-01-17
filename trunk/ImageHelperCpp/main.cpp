#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "detect.h"

using namespace cv;
using namespace std;

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
    result = helper::detectLines(argv[2], float(atof(argv[4])), atoi(argv[3]), argv[5]);
    break;

  default:
    cout << "Error: Wrong mode." << endl;
    result = -1;
    break;

  }

  waitKey(0);
  return result;
}