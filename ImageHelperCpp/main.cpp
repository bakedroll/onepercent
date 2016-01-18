#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "detect.h"
#include "draw.h"
#include "reduce.h"
#include "io.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
  if (argc < 4)
  {
    cout << "Error: Not enough parameters." << endl;
    return -1;
  }

  int result = 0;
  int mode = atoi(argv[1]);
  
  if (mode == 0)
  {
    const char* inputImage = argv[2];
    float displayScale = float(atof(argv[4]));
    int depth = atoi(argv[3]);
    const char* outputPoly = argv[5];

    helper::Graph graph;
    helper::detectLines(inputImage, displayScale, depth, graph);
    helper::reducePoints(graph);
    helper::writePolyFile(graph, outputPoly);

  }
  else
  {
    cout << "Error: Wrong mode." << endl;
    result = -1;
  }

  waitKey(0);
  return result;
}