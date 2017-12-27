#pragma once

#include "types.h"

namespace helper
{
  void makeDistanceMap(Graph& graph, cv::Mat& result, float scale, float maxDistance);
}