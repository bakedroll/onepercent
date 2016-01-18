#pragma once

#include "types.h"

namespace helper
{
  void detectLines(cv::Mat& inputImage, cv::Mat& displayImage, int depth, Graph& outGraph);
}