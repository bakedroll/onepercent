#include "draw.h"

#include <opencv2/imgproc/imgproc.hpp>

namespace helper
{
  void drawLine(cv::Mat& mat, Graph& graph, int p1, int p2, float scale, cv::Scalar scalar)
  {
    line(mat, graph.points.find(p1)->second * scale, graph.points.find(p2)->second * scale, scalar);
  }

  void drawGraph(cv::Mat& mat, Graph& graph, float scale)
  {
    mat.setTo(cv::Scalar(0, 0, 0));

    for (TriangleList::iterator tIt = graph.triangles.begin(); tIt != graph.triangles.end(); ++tIt)
    {
      drawLine(mat, graph, tIt->idx[0], tIt->idx[1], scale, cv::Scalar(0, 255, 0));
      drawLine(mat, graph, tIt->idx[1], tIt->idx[2], scale, cv::Scalar(0, 255, 0));
      drawLine(mat, graph, tIt->idx[2], tIt->idx[0], scale, cv::Scalar(0, 255, 0));
    }

    for (EdgeValueList::iterator eIt = graph.edges.begin(); eIt != graph.edges.end(); ++eIt)
      drawLine(mat, graph, eIt->first.first, eIt->first.second, scale, cv::Scalar(0, 0, eIt->second));

    for (IdPointMap::iterator rIt = graph.points.begin(); rIt != graph.points.end(); ++rIt)
      mat.at<cv::Vec3b>(rIt->second * scale) = cv::Vec3b(255, 0, 0);
  }
}