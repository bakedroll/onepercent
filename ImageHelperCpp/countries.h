#pragma once

#include "types.h"

namespace helper
{
  void makeCountries(Graph& graph, CyclesMap& cycles, CountriesTable& table, float mapscale, CountriesMap& countries, cv::Mat& countriesMap);
}