#include "draw.h"

#include <opencv2/imgproc/imgproc.hpp>

namespace helper
{
  void drawEdge(cv::Mat& mat, Graph& graph, int p1, int p2, float scale, cv::Scalar scalar)
  {
    line(mat, graph.points.find(p1)->second * scale, graph.points.find(p2)->second * scale, scalar);
  }

  void drawTriangle(cv::Mat& mat, Graph& graph, int* idx, float scale, cv::Scalar scalar)
  {
    drawEdge(mat, graph, idx[0], idx[1], scale, scalar);
    drawEdge(mat, graph, idx[1], idx[2], scale, scalar);
    drawEdge(mat, graph, idx[2], idx[0], scale, scalar);
  }

  void drawGraph(cv::Mat& mat, Graph& graph, float scale, bool drawTriangles)
  {
    mat.setTo(cv::Scalar(0, 0, 0));

    if (drawTriangles)
      for (TriangleList::iterator tIt = graph.triangles.begin(); tIt != graph.triangles.end(); ++tIt)
        drawTriangle(mat, graph, tIt->idx, scale, cv::Scalar(0, 172, 0));

    for (EdgeValueList::iterator eIt = graph.edges.begin(); eIt != graph.edges.end(); ++eIt)
      drawEdge(mat, graph, eIt->first.first, eIt->first.second, scale, cv::Scalar(0, 0, eIt->second));

    for (IdPointMap::iterator rIt = graph.points.begin(); rIt != graph.points.end(); ++rIt)
      mat.at<cv::Vec3b>(rIt->second * scale) = cv::Vec3b(255, 0, 0);
  }

  void drawCycles(cv::Mat& mat, Graph& graph, Cycles& cycles, float scale)
  {
    mat.setTo(cv::Scalar(0, 0, 0));

    for (Cycles::iterator it = cycles.begin(); it != cycles.end(); ++it)
    {
      cv::Scalar color(rand() & 155 + 100, rand() & 155 + 100, rand() & 155 + 100);

      for (TriangleList::iterator tit = it->trianlges.begin(); tit != it->trianlges.end(); ++tit)
        drawTriangle(mat, graph, tit->idx, scale, color);
    }
  }
}