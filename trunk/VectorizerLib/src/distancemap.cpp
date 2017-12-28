#include "vectorizer/distancemap.h"
#include "vectorizer/quadtree.h"
#include "vectorizer/io.h"

namespace helper
{
  void checkRadius(Graph& graph, QuadTreeNode<int>& quadtree, cv::Mat& img, float x, float y, int ix, int iy, float maxDistance, float radius)
  {
    BoundingBox<float> bb(cv::Point2f(x - radius, y - radius), cv::Point2f(x + radius, y + radius));
    QuadTreeNode<int>::Data data;

    quadtree.gatherDataWithinBoundary(bb, data);
    if (data.size() == 0)
    {
      checkRadius(graph, quadtree, img, x, y, ix, iy, maxDistance, radius * 2.0f);
      return;
    }

    std::map<float, int> nearestPoints;
    for (QuadTreeNode<int>::Data::iterator it = data.begin(); it != data.end(); ++it)
    {
      float dx = it->point().x - x;
      float dy = it->point().y - y;
      float distance2 = dx * dx + dy * dy;
      nearestPoints[distance2] = it->data();
    }

    int nearestPointId = nearestPoints.begin()->second;
    float dist2 = nearestPoints.begin()->first;

    float dist = sqrt(dist2);
    if (dist > maxDistance)
    {
      img.at<uchar>(cv::Point2i(ix, iy)) = 0;
    }
    else
    {
      img.at<uchar>(cv::Point2i(ix, iy)) = 255 - uchar(255.0f * dist / maxDistance);
    }
  }

  void makeDistanceMap(Graph& graph, cv::Mat& result, float scale, float maxDistance)
  {
    ProgressPrinter progress("Make distance map");

    int height = int(graph.boundary.height() * scale);
    int width = int(graph.boundary.width() * scale);

    result = cv::Mat(height, width, CV_8UC1);

    NeighbourMap neighbours;
    neighbourMapFromEdges(graph.edges, neighbours);

    QuadTreeNode<int> quadtree(graph.boundary, 10);
    for (IdPointMap::iterator it = graph.points.begin(); it != graph.points.end(); ++it)
      quadtree.insert(QuadTreeNodeData<int>(it->second, it->first));

    int max = width * height;
    int i = 0;

    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        float px = float(x) / scale;
        float py = float(y) / scale;

        checkRadius(graph, quadtree, result, px, py, x, y, maxDistance, maxDistance);

        i++;
        progress.update(i, max);
      }
    }
  }
}