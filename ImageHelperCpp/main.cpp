#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include "detect.h"
#include "reduce.h"
#include "io.h"
#include "draw.h"
#include "check.h"

void triangulate(const char* triangleCommand, const char* polyFilename)
{
  std::string fn(polyFilename);
  fn += ".0.poly";

  std::string command(triangleCommand);
  command += " -pq " + fn;

  std::system(command.c_str());
}

int detectLines(int argc, char** argv)
{
  if (argc < 8)
  {
    std::cout << "Error: Not enough parameters." << std::endl;
    return -1;
  }

  const char* inputImage = argv[2];
  float displayScale = float(atof(argv[4]));
  int depth = atoi(argv[3]);
  float reduce = float(atof(argv[5]));
  const char* polyfile = argv[6];
  const char* triangleCommand = argv[7];

  cv::Mat image;

  image = cv::imread(inputImage, cv::IMREAD_GRAYSCALE);

  if (!image.data)
  {
    printf("Error: Image could not be loaded: %s\n", inputImage);
    return -1;
  }

  cv::Mat displayImage(image.rows, image.cols, CV_8UC3);
  cv::Mat resultImage(int(image.rows * displayScale), int(image.cols * displayScale), CV_8UC3);
  cv::Mat finalImage(int(image.rows * displayScale), int(image.cols * displayScale), CV_8UC3);

  cvtColor(image, displayImage, CV_GRAY2RGB);

  helper::Graph graph;
  helper::detectLines(image, displayImage, depth, graph);

  helper::checkDuplicates(graph);

  helper::reducePoints(graph, reduce);

  helper::checkDuplicates(graph);

  helper::writePolyFile(graph, polyfile);
  helper::drawGraph(resultImage, graph, displayScale);

  triangulate(triangleCommand, polyfile);

  helper::Graph triGraph;
  helper::readGraphFiles(triGraph, polyfile, 1);

  helper::drawGraph(finalImage, triGraph, displayScale);

  imshow("Lines", displayImage);
  imshow("Result", resultImage);
  imshow("Final", finalImage);
  imwrite("img.png", displayImage);
  imwrite("result.png", resultImage);
  imwrite("final.png", finalImage);

  return 0;
}

int main(int argc, char** argv)
{
  int result;
  int mode = atoi(argv[1]);
  
  if (mode == 0)
  {
    result = detectLines(argc, argv);
  }
  else
  {
    std::cout << "Error: Wrong mode." << std::endl;
    result = -1;
  }

  cv::waitKey(0);
  return result;
}