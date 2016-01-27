#pragma once

#include "types.h"
#include "findcycles.h"

namespace helper
{
  void drawGraph(cv::Mat& mat, Graph& graph, float scale = 1.0f);
  void drawCycles(cv::Mat& mat, Graph& graph, Cycles& cycles, float scale = 1.0f);
}