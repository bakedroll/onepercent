#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "detect.h"
#include "reduce.h"
#include "io.h"
#include "draw.h"
#include "check.h"
#include "findcycles.h"
#include "mesh.h"
#include "countries.h"

typedef std::vector<std::string> StringList;
typedef std::map<std::string, StringList> ArgumentMap;

void parseParameters(int argc, char** argv, ArgumentMap& arguments)
{
  std::string lastArg = "";
  StringList lastParams;

  bool justAdded = false;
  for (int i = 1; i < argc; i++)
  {
    std::string arg(argv[i]);
    if (*arg.begin() == '-')
    {
      arg.erase(arg.begin());

      if (!lastArg.empty())
      {
        if (arguments.find(arg) != arguments.end())
        {
          printf("Warning: Duplicate argument: %s\n", argv[i]);
        }
        else
        {
          arguments.insert(ArgumentMap::value_type(lastArg, lastParams));
          justAdded = true;
        }

        lastParams.clear();
      }

      lastArg = arg;
    }
    else
    {
      justAdded = false;
      if (lastArg.empty())
      {
        printf("Warning: Unexpected argument: %s\n", argv[i]);
        continue;
      }

      lastParams.push_back(arg);
    }
  }

  if (!justAdded)
    arguments.insert(ArgumentMap::value_type(lastArg, lastParams));
}

std::string getStringArgument(ArgumentMap& arguments, std::string key, bool required = false)
{
  ArgumentMap::iterator it = arguments.find(key);
  if (it == arguments.end() || it->second.size() < 1)
  {
    if (required)
      throw std::exception(key.c_str());

    return std::string();
  }

  return it->second[0];
}

bool getBoolArgument(ArgumentMap& arguments, std::string key)
{
  ArgumentMap::iterator it = arguments.find(key);
  if (it == arguments.end())
    return false;

  return true;
}

int getIntArgument(ArgumentMap& arguments, std::string key, int defaultVal, bool required = false)
{
  std::string result = getStringArgument(arguments, key, required);

  if (result.empty())
    return defaultVal;

  return atoi(result.c_str());
}

float getFloatArgument(ArgumentMap& arguments, std::string key, float defaultVal, bool required = false)
{
  std::string result = getStringArgument(arguments, key, required);

  if (result.empty())
    return defaultVal;

  return float(atof(result.c_str()));
}

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
  ArgumentMap arguments;
  parseParameters(argc, argv, arguments);

  std::string inputimage;
  std::string dbgimage;
  std::string polyfile;
  std::string trianglecommand;
  std::string outputFilename;
  std::string countriesTableFilename;
  std::string countriesFilename;
  float displayScale, reduce;
  int depth, minAngle;
  bool useThres, dbgCycles;
  float thickness;
  float shift;
  float countriesMapScale;

  try
  {
    inputimage = getStringArgument(arguments, "i", true);
    dbgimage = getStringArgument(arguments, "D", false);
    polyfile = getStringArgument(arguments, "p", true);
    trianglecommand = getStringArgument(arguments, "t", true);
    outputFilename = getStringArgument(arguments, "o", true);
    countriesTableFilename = getStringArgument(arguments, "c", false);
    countriesFilename = getStringArgument(arguments, "co", false);
    displayScale = getFloatArgument(arguments, "s", 1.0f);
    reduce = getFloatArgument(arguments, "r", 0.7f);
    depth = getIntArgument(arguments, "d", 10);
    minAngle = getIntArgument(arguments, "a", 15);
    useThres = getBoolArgument(arguments, "T");
    dbgCycles = getBoolArgument(arguments, "C");
    thickness = getFloatArgument(arguments, "l", 0.005f);
    shift = getFloatArgument(arguments, "S", 255.0f);
    countriesMapScale = getFloatArgument(arguments, "ms", 0.25f);
  }
  catch (std::exception& e)
  {
    printf("Error: Missing argument: %s\n", e.what());
    return -1;
  }

  printf("Detecting countries\n");

  printf("Depth (edge length): %d\n", depth);
  printf("Reduce: %f percent\n", reduce * 100.0f);
  printf("Min angle: %d\n\n", minAngle);

  cv::Mat image;

  image = cv::imread(inputimage, cv::IMREAD_GRAYSCALE);

  if (!image.data)
  {
    printf("Error: Image could not be loaded: %s\n", inputimage.c_str());
    return -1;
  }

  cv::Mat displayImage(image.rows, image.cols, CV_8UC3);
  cv::Mat resultImage(int(image.rows * displayScale), int(image.cols * displayScale), CV_8UC3);
  cv::Mat finalImage(int(image.rows * displayScale), int(image.cols * displayScale), CV_8UC3);
  cv::Mat cycleImage(int(image.rows * displayScale), int(image.cols * displayScale), CV_8UC3);
  cv::Mat filledCycleImage(int(image.rows * displayScale), int(image.cols * displayScale), CV_8UC3);

  cvtColor(image, displayImage, CV_GRAY2RGB);

  if (useThres)
    cv::threshold(image, image, 30, 255, CV_THRESH_BINARY);

  // detecting lines
  helper::Graph graph;
  helper::detectLines(image, displayImage, depth, graph);

  // check for duplicates
  helper::removeDuplicates(graph);

  // remove dead ends
  helper::removeDeadEnds(graph);

  // reduce
  helper::reducePoints(graph, reduce);

  // write to poly file and draw debug image
  helper::writePolyFile(graph, polyfile.c_str());
  helper::drawGraph(resultImage, graph, displayScale);

  // triangulate
  triangulate(trianglecommand.c_str(), polyfile.c_str(), minAngle);

  // read result and draw debug image
  helper::Graph triGraph;
  helper::readGraphFiles(triGraph, polyfile.c_str(), 1);

  // remove single points
  //helper::removeSinglePoints(graph);

  helper::drawGraph(finalImage, triGraph, displayScale);

  triGraph.boundary = graph.boundary;

  // find cycles
  helper::CyclesMap cycles;
  helper::findCycles(triGraph, cycles, dbgCycles, displayScale);
  helper::drawCycles(cycleImage, triGraph, cycles, displayScale);
  helper::drawFilledCycles(filledCycleImage, triGraph, cycles, displayScale);
  helper::drawCycleNumbers(filledCycleImage, triGraph, cycles, displayScale);

  // make mesh
  helper::SphericalMesh mesh;
  helper::makeSphericalMesh(triGraph, mesh, thickness, shift);
  helper::writeBoundariesFile(mesh, outputFilename.c_str());

  if (!countriesTableFilename.empty() && !countriesFilename.empty())
  {
    // make countries
    helper::CountriesTable table;
    helper::readCountriesTable(countriesTableFilename.c_str(), table);

    helper::CountriesMap countries;
    cv::Mat countriesMap;
    helper::makeCountries(triGraph, cycles, table, countriesMapScale, countries, countriesMap);
    helper::writeCountriesFile(countriesFilename.c_str(), triGraph, countries, countriesMap, triGraph.boundary, shift);
  }
  
  if (!dbgimage.empty())
  {
    imshow("Lines", displayImage);
    imshow("Result", resultImage);
    imshow("Final", finalImage);
    imshow("Cycles", cycleImage);
    imshow("Filled cycles", filledCycleImage);
    imwrite(std::string(dbgimage) + ".steps.png", displayImage);
    imwrite(std::string(dbgimage) + ".edges.png", resultImage);
    imwrite(std::string(dbgimage) + ".triangles.png", finalImage);
    imwrite(std::string(dbgimage) + ".cycles.png", cycleImage);
    imwrite(std::string(dbgimage) + ".filled.png", filledCycleImage);
  }

  return 0;
}

int main(int argc, char** argv)
{
  int result;
  result = detectLines(argc, argv);

  cv::waitKey(0);
  return result;
}