#include "vectorizer/distancemap.h"
#include "vectorizer/octtree.h"
#include "vectorizer/io.h"
#include "vectorizer/types.h"

namespace helper
{
  void checkRadius(Graph& graph, OctTreeNode<int>& octtree, cv::Mat& img, float x, float y, int ix, int iy, int width, int height, float maxDistance, float radius)
  {
    osg::Vec3f p3d;
    osg::Vec3f r(radius, radius, radius);
    makeCartesianPoint(cv::Point2f(float(ix), float(iy)), float(width), float(height), p3d, 500.0f, 0.0f);

    osg::BoundingBox bb(p3d - r, p3d + r);
    OctTreeNode<int>::Data data;

    octtree.gatherDataWithinBoundary(bb, data);
    if (data.size() == 0)
    {
      checkRadius(graph, octtree, img, x, y, ix, iy, width, height, maxDistance, radius * 2.0f);
      return;
    }

    std::map<float, int> nearestPoints;
    for (OctTreeNode<int>::Data::iterator it = data.begin(); it != data.end(); ++it)
    {
      osg::Vec3f delta = it->point() - p3d;
      nearestPoints[delta.length2()] = it->data();
    }

    // int nearestPointId = nearestPoints.begin()->second;
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

    IdPoint3DMap points3D;
    makeCartesianPoints(graph, points3D, 500.0f, 0.0f);

    osg::BoundingBox bb(osg::Vec3f(-600.0f, -600.0f, -600.0f), osg::Vec3f(600.0f, 600.0f, 600.0f));
    OctTreeNode<int> octtree(bb, 10);
    for (IdPoint3DMap::iterator it = points3D.begin(); it != points3D.end(); ++it)
    {
      helper::Point3D p3d = it->second;
      if (p3d.originId > -1 || neighbours.count(it->first) == 0)
        continue;

      osg::Vec3f p(p3d.value[0], p3d.value[1], p3d.value[2]);
      octtree.insert(OctTreeNodeData<int>(p, it->first));
    }

    int max = width * height;
    int i = 0;

    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        float px = float(x) / scale;
        float py = float(y) / scale;

        checkRadius(graph, octtree, result, px, py, x, y, width, height, maxDistance, maxDistance);

        i++;
        progress.update(i, max);
      }
    }
  }
}