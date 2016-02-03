#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include "detect.h"
#include "reduce.h"
#include "io.h"
#include "draw.h"
#include "check.h"
#include "findcycles.h"
#include "quadtree.h"

void triangulate(const char* triangleCommand, const char* polyFilename, int minAngle = 20)
{
  std::string fn(polyFilename);
  fn += ".0.poly";

  std::string command(triangleCommand);
  command += " -pq" + std::to_string(minAngle) + " " + fn;

  std::system(command.c_str());
}

int detectLines(int argc, char** argv)
{
  if (argc < 11)
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
  int minAngle = atoi(argv[8]);
  const char* dbgImage = argv[9];
  int useThres = atoi(argv[10]);

  printf("Triangulating...\n");
  printf("Depth (edge length): %d\n", depth);
  printf("Reduce: %f percent\n", reduce * 100.0f);
  printf("Min angle: %d\n\n", minAngle);

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
  cv::Mat cycleImage(int(image.rows * displayScale), int(image.cols * displayScale), CV_8UC3);

  cvtColor(image, displayImage, CV_GRAY2RGB);

  if (useThres)
    cv::threshold(image, image, 30, 255, CV_THRESH_BINARY);

  printf("Detecting lines\n");

  // detecting lines
  helper::Graph graph;
  helper::detectLines(image, displayImage, depth, graph);

  // reducing points and double checking for duplicates
  helper::checkDuplicatesAndRemove(graph);

  printf("Reduce\n");
  helper::reducePoints(graph, reduce);
  helper::checkDuplicatesAndRemove(graph);

  printf("Write to poly file\n");

  // write to poly file and draw debug image
  helper::writePolyFile(graph, polyfile);
  helper::drawGraph(resultImage, graph, displayScale);

  // triangulate
  triangulate(triangleCommand, polyfile, minAngle);

  printf("Read from poly file\n");

  // read result and draw debug image
  helper::Graph triGraph;
  helper::readGraphFiles(triGraph, polyfile, 1);
  helper::drawGraph(finalImage, triGraph, displayScale);

  triGraph.boundary = graph.boundary;

  printf("Detecting cycles\n");

  // find cycles
  helper::Cycles cycles;
  helper::findCycles(triGraph, cycles); //, false, image.rows, image.cols, displayScale);
  helper::drawCycles(cycleImage, triGraph, cycles, displayScale);
  
  imshow("Lines", displayImage);
  imshow("Result", resultImage);
  imshow("Final", finalImage);
  imshow("Cycles", cycleImage);
  imwrite(std::string(dbgImage) + ".steps.png", displayImage);
  imwrite(std::string(dbgImage) + ".edges.png", resultImage);
  imwrite(std::string(dbgImage) + ".triangles.png", finalImage);
  imwrite(std::string(dbgImage) + ".cycles.png", cycleImage);

  return 0;
}

int main(int argc, char** argv)
{
  int result = 0;
  int mode = atoi(argv[1]);

  int width = 300;
  int height = 300;

  cv::Mat display(width, height, CV_8UC3);
  display.setTo(cv::Vec3b(0, 0, 0));
  
  if (mode == 0)
  {
    result = detectLines(argc, argv);

    helper::QuadTreeNode<int> tree(helper::BoundingBox<float>(cv::Point2f(0.0f, 0.0f), cv::Point2f(float(width), float(height))), 10);

    helper::PointListi points;
    for (int i = 0; i < 1000; i++)
    {
      helper::QuadTreeNodeData<int> data(cv::Point2f(float(rand() % width), float(rand() % height)), 0);
      tree.insert(data);
      points.push_back(data.point());
    }

    helper::QuadTreeNode<int>::Data resultData;
    tree.gatherDataWithinBoundary(helper::BoundingBox<float>(cv::Point2f(100.0f, 56.0f), cv::Point2f(290.0f, 250.0f)), resultData);

    helper::PointListi results;
    for (helper::QuadTreeNode<int>::Data::iterator it = resultData.begin(); it != resultData.end(); ++it)
      results.push_back(it->point());

    helper::drawPoints(display, points, 1.0f, cv::Vec3b(0, 100, 0));
    helper::drawPoints(display, results, 1.0f, cv::Vec3b(200, 0, 0));
    imshow("QuadTree", display);
  }
  else
  {
    std::cout << "Error: Wrong mode." << std::endl;
    result = -1;
  }

  cv::waitKey(0);
  return result;
}