#pragma once

#include "types.h"

#include <chrono>

namespace helper
{
  class ProgressPrinter
  {
  public:
    ProgressPrinter(std::string name);

    void update(long long p, long long max);

  private:
    std::string m_name;
    long long m_percent;
    clock_t m_lastUpdate;

    void printCurrent();
  };

  void writePolyFile(Graph& graph, const char* filename);
  void writeBoundariesFile(Graph& graph, BoundariesMeshData& meshdata, const char* filename);

  void readGraphFiles(Graph& graph, const char* filename, int iteration);

  void readCountriesTable(const char* filename, CountriesTable& countries);
  void writeCountriesFile(const char* filename, Graph& graph, CountriesMap& countries, cv::Mat& countriesMap, float shift);
}