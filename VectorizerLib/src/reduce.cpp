#include "vectorizer/reduce.h"
#include "vectorizer/io.h"

namespace helper
{
  double angleBetween(cv::Vec2f pend1, cv::Vec2f pcenter, cv::Vec2f pend2)
  {
    cv::Vec2d v1(pend1 - pcenter);
    cv::Vec2d v2(pend2 - pcenter);

    double l1 = sqrt(v1.val[0] * v1.val[0] + v1.val[1] * v1.val[1]);
    double l2 = sqrt(v2.val[0] * v2.val[0] + v2.val[1] * v2.val[1]);

    v1.val[0] /= l1;
    v1.val[1] /= l1;

    v2.val[0] /= l2;
    v2.val[1] /= l2;

    return acos(v1.ddot(v2));
  }

  void removeFromGraph(Graph& graph, int pointId)
  {
    graph.points.erase(graph.points.find(pointId));

    EdgeValueList::iterator it = graph.edges.begin();
    while (it != graph.edges.end())
    {
      if (it->first.first == pointId || it->first.second == pointId)
        it = graph.edges.erase(it);
      else
        ++it;
    }
  }

  void calculateAngle(NeighbourMap::iterator& it, AnglePointMap& angles, Graph& graph)
  {
    // two neighbours with same value
    if (it->second.size() == 2 && it->second[0].second == it->second[1].second)
    {
      double angle = angleBetween(graph.points[it->second[0].first], graph.points[it->first], graph.points[it->second[1].first]);
      angles.insert(AnglePointMap::value_type(std::abs(CV_PI - angle), it->first));
    }
  }

  void removeAngle(AnglePointMap& angles, int pointId)
  {
    for (AnglePointMap::iterator it = angles.begin(); it != angles.end(); ++it)
    {
      if (it->second == pointId)
      {
        angles.erase(it);
        return;
      }
    }
  }

  void reducePoints(Graph& graph, float percentPoints)
  {
    ProgressPrinter progress("Reduce");

    NeighbourMap neighbourMap;
    AnglePointMap angles;
    IdSet ignore;

    neighbourMapFromEdges(graph.edges, neighbourMap);

    for (NeighbourMap::iterator it = neighbourMap.begin(); it != neighbourMap.end(); ++it)
      calculateAngle(it, angles, graph);

    int nPoints = std::min(int(graph.points.size() * (1.0f - percentPoints)), int(angles.size()));

    for (int i = 0; i < nPoints; i++)
    {
      AnglePointMap::iterator firstIt = angles.begin();
      while (firstIt != angles.end() && ignore.find(firstIt->second) != ignore.end())
      {
        ++firstIt;
      }

      if (firstIt == angles.end())
        break;

      int pointId = firstIt->second;

      // remove from helper structured
      int e1, e2;
      uchar value;
      if (!removeNeighbourMapPoint(neighbourMap, pointId, e1, e2, value))
      {
        ignore.insert(pointId);
        i--;
        continue;
      }

      angles.erase(firstIt);

      // remove from graph
      removeFromGraph(graph, pointId);

      // add new edge to graph
      graph.edges.push_back(EdgeValue(Edge(e1, e2), value));

      // add to neighbour map
      NeighbourMap::iterator e1it = neighbourMap.find(e1);
      NeighbourMap::iterator e2it = neighbourMap.find(e2);

      e1it->second.push_back(NeighbourValue(e2, value));
      e2it->second.push_back(NeighbourValue(e1, value));

      // update angles
      removeAngle(angles, e1);
      removeAngle(angles, e2);

      calculateAngle(e1it, angles, graph);
      calculateAngle(e2it, angles, graph);

      progress.update(i, nPoints - 1);
    }
  }
}