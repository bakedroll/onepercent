#include "vectorizer/distancemap.h"
#include "vectorizer/octtree.h"
#include "vectorizer/io.h"
#include "vectorizer/types.h"

#define C_RADIUS 500.0f

namespace helper
{
  float getArcLength(osg::Vec3f p1, osg::Vec3f p2)
  {
    return C_RADIUS * acos((p1 * p2) / (p1.length() * p2.length()));
  }

  float distanceTo(IdPoint3DMap& points3D, const osg::Vec3f& point, int p1Id, int p2Id)
  {
    Point3D& p13D = points3D[p1Id];
    Point3D& p23D = points3D[p2Id];

    osg::Vec3f p1(p13D.value[0], p13D.value[1], p13D.value[2]);
    osg::Vec3f p2(p23D.value[0], p23D.value[1], p23D.value[2]);

    osg::Vec3f dir = p2 - p1;
    osg::Vec3f dirnorm = dir;
    dirnorm.normalize();

    if (dir * (point - p1) < 0.0f)
      return getArcLength(point, p1);

    if (-dir * (point - p2) < 0.0f)
      return getArcLength(point, p2);

    float dist2 = (dir ^ (p1 - point)).length2() / dir.length2();
    osg::Vec3f pointOnEdge = p1 + dirnorm * sqrt((p1 - point).length2() - dist2);

    pointOnEdge.normalize();
    pointOnEdge *= C_RADIUS;

    return getArcLength(point, pointOnEdge);
  }

  void checkRadius(IdPoint3DMap& points3D, NeighbourMap& neighbours, OctTreeNode<int>& octtree, cv::Mat& img, float x, float y, int ix, int iy, int width, int height, float maxDistance, float radius)
  {
    osg::Vec3f p3d;
    osg::Vec3f r(radius, radius, radius);
    makeCartesianPoint(cv::Point2f(float(ix), float(iy)), float(width), float(height), p3d, C_RADIUS, 0.0f);

    osg::BoundingBox bb(p3d - r, p3d + r);
    OctTreeNode<int>::Data data;

    octtree.gatherDataWithinBoundary(bb, data);
    if (data.size() == 0)
    {
      checkRadius(points3D, neighbours, octtree, img, x, y, ix, iy, width, height, maxDistance, radius * 2.0f);
      return;
    }

    std::map<float, int> nearestPoints;
    for (OctTreeNode<int>::Data::iterator it = data.begin(); it != data.end(); ++it)
    {
      osg::Vec3f delta = it->point() - p3d;
      nearestPoints[delta.length2()] = it->data();
    }

    float dist = std::numeric_limits<float>::max();
    for (std::map<float, int>::iterator nit = nearestPoints.begin(); nit != nearestPoints.end(); ++nit)
    {
      NeighbourValueList& nlist = neighbours[nit->second];
      assert(nlist.size() > 0);
      for (NeighbourValueList::iterator it = nlist.begin(); it != nlist.end(); ++it)
        dist = std::min<float>(dist, distanceTo(points3D, p3d, nit->second, it->first));
    }

    //float dist = sqrt(dist2);
    if (dist > maxDistance)
      img.at<uchar>(cv::Point2i(ix, iy)) = 0;
    else
      img.at<uchar>(cv::Point2i(ix, iy)) = 255 - uchar(255.0f * dist / maxDistance);
  }

  void makeDistanceMap(Graph& graph, cv::Mat& result, float scale, float maxDistance, cv::Mat& countriesMap)
  {
    ProgressPrinter progress("Make distance map");

    int height = int(graph.boundary.height() * scale);
    int width = int(graph.boundary.width() * scale);

    bool bOptimze = true;
    if (countriesMap.cols < width)
    {
      bOptimze = false;
      printf("Warning: Countries map smaller than distant map. Cannot be used for optimization.\n");
    }

    result = cv::Mat(height, width, CV_8UC1);

    NeighbourMap neighbours;
    neighbourMapFromEdges(graph.edges, neighbours);

    IdPoint3DMap points3D;
    makeCartesianPoints(graph, points3D, C_RADIUS, 0.0f);

    float bbHalfLength = C_RADIUS * 1.1f;

    osg::BoundingBox bb(osg::Vec3f(-bbHalfLength, -bbHalfLength, -bbHalfLength), osg::Vec3f(bbHalfLength, bbHalfLength, bbHalfLength));
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
        //if (!bOptimze || countriesMap.at<uchar>(y * countriesMap.rows / height, x * countriesMap.cols / width) > 0)
        {
          float px = float(x) / scale;
          float py = float(y) / scale;

          checkRadius(points3D, neighbours, octtree, result, px, py, x, y, width, height, maxDistance, maxDistance);
        }
        /*else
        {
          result.at<uchar>(cv::Point2i(x, y)) = 0;
        }*/

        i++;
        progress.update(i, max);
      }
    }
  }
}