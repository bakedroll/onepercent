#pragma once

#include "types.h"
#include "findcycles.h"

namespace helper
{
  void drawPoints(cv::Mat& mat, PointListi points, float scale, cv::Vec3b color);
  void drawEdge(cv::Mat& mat, Graph& graph, int p1, int p2, float scale, cv::Scalar scalar);
  void drawGraph(cv::Mat& mat, Graph& graph, float scale = 1.0f, bool drawTriangles = true);
  void drawCycles(cv::Mat& mat, Graph& graph, CyclesMap& cycles, float scale = 1.0f);
  void drawFilledTriangle(cv::Mat& mat, Graph& graph, int* idx, float scale, cv::Scalar color);
  void drawFilledCycles(cv::Mat& mat, Graph& graph, CyclesMap& cycles, float scale = 1.0f);
  void drawCycleNumbers(cv::Mat& map, Graph& graph, CyclesMap& cycles, float scale = 1.0f);
}