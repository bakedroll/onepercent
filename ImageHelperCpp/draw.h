#pragma once

#include "types.h"
#include "findcycles.h"

namespace helper
{
  void drawEdge(cv::Mat& mat, Graph& graph, int p1, int p2, float scale, cv::Scalar scalar);
  void drawGraph(cv::Mat& mat, Graph& graph, float scale = 1.0f, bool drawTriangles = true);
  void drawCycles(cv::Mat& mat, Graph& graph, Cycles& cycles, float scale = 1.0f);
}