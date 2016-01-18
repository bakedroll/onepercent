#include "draw.h"

#include <opencv2/imgproc/imgproc.hpp>

namespace helper
{
  void drawGraph(cv::Mat& mat, Graph& graph, float scale)
  {
    mat.setTo(cv::Scalar(0, 0, 0));

    for (EdgeValueList::iterator eIt = graph.edges.begin(); eIt != graph.edges.end(); ++eIt)
      line(mat, graph.points.find(eIt->first.first)->second * scale, graph.points.find(eIt->first.second)->second * scale, cv::Scalar(0, 0, eIt->second));

    for (IdPointMap::iterator rIt = graph.points.begin(); rIt != graph.points.end(); ++rIt)
      mat.at<cv::Vec3b>(rIt->second * scale) = cv::Vec3b(255, 0, 0);
  }
}