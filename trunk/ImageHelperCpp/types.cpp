#include "types.h"

namespace helper
{
  BoundingBox::BoundingBox()
  {

  }

  BoundingBox::BoundingBox(PointList& points)
  {
    m_min = cv::Point2f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    m_max = cv::Point2f(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

    for (PointList::iterator it = points.begin(); it != points.end(); ++it)
    {
      m_min.x = std::min(m_min.x, it->x);
      m_min.y = std::min(m_min.y, it->y);
      m_max.x = std::max(m_max.x, it->x);
      m_max.y = std::max(m_max.y, it->y);
    }
  }

  BoundingBox::BoundingBox(Graph& graph, PointSet& points)
  {
    m_min = cv::Point2f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    m_max = cv::Point2f(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

    for (PointSet::iterator it = points.begin(); it != points.end(); ++it)
    {
      IdPointMap::iterator pit = graph.points.find(*it);

      m_min.x = std::min(m_min.x, pit->second.x);
      m_min.y = std::min(m_min.y, pit->second.y);
      m_max.x = std::max(m_max.x, pit->second.x);
      m_max.y = std::max(m_max.y, pit->second.y);
    }
  }

  BoundingBox::BoundingBox(cv::Point2f min, cv::Point2f max)
    : m_min(min)
    , m_max(max)
  {

  }

  cv::Point2f BoundingBox::min() const
  {
    return m_min;
  }

  cv::Point2f BoundingBox::max() const
  {
    return m_max;
  }

  float BoundingBox::width() const
  {
    return m_max.x - m_min.x + 1.0f;
  }

  float BoundingBox::height() const
  {
    return m_max.y - m_min.y + 1.0f;
  }

  cv::Point2f BoundingBox::center() const
  {
    return m_min + m_max / 2.0f;
  }

  void insertNeighbour(NeighbourMap& map, int p1, int p2, uchar attr)
  {
    NeighbourMap::iterator it = map.find(p1);

    NeighbourValue value;
    value.first = p2;
    value.second = attr;

    if (it == map.end())
    {
      NeighbourValueList list;
      list.push_back(value);
      map.insert(NeighbourMap::value_type(p1, list));
    }
    else
    {
      it->second.push_back(value);
    }
  }

  void neighbourMapFromEdges(EdgeValueList& edges, NeighbourMap& neighbourMap)
  {
    for (EdgeValueList::iterator eit = edges.begin(); eit != edges.end(); ++eit)
    {
      insertNeighbour(neighbourMap, eit->first.first, eit->first.second, eit->second);
      insertNeighbour(neighbourMap, eit->first.second, eit->first.first, eit->second);
    }
  }

  void removeNeighbourFromList(NeighbourValueList& list, int id)
  {
    for (NeighbourValueList::iterator it = list.begin(); it != list.end(); ++it)
    {
      if (it->first == id)
      {
        list.erase(it);
        return;
      }
    }
  }

  void removeNeighbourMapPoint(NeighbourMap& neighbourMap, int pointId, int& endpoint1, int& endpoint2, uchar& value)
  {
    NeighbourMap::iterator it = neighbourMap.find(pointId);

    assert(it->second.size() == 2);
    assert(it->second[0].second == it->second[1].second);

    value = it->second[0].second;
    endpoint1 = it->second[0].first;
    endpoint2 = it->second[1].first;

    neighbourMap.erase(it);

    removeNeighbourFromList(neighbourMap.find(endpoint1)->second, pointId);
    removeNeighbourFromList(neighbourMap.find(endpoint2)->second, pointId);
  }

  void makeFloatFloatIdMap(Graph& graph, FloatFloatIdMap& map)
  {
    for (IdPointMap::iterator it = graph.points.begin(); it != graph.points.end(); ++it)
    {
      FloatFloatIdMap::iterator itx = map.find(it->second.x);
      if (itx != map.end())
      {
        FloatIdMap::iterator ity = itx->second.find(it->second.y);
        if (ity != itx->second.end())
          continue;

        itx->second.insert(FloatIdMap::value_type(it->second.y, it->first));
      }
      else
      {
        FloatIdMap fmap;
        fmap.insert(FloatIdMap::value_type(it->second.y, it->first));
        map.insert(FloatFloatIdMap::value_type(it->second.x, fmap));
      }
    }
  }

  void makePointTriangleMap(Graph& graph, PointTriangleMap& map)
  {
    for (TriangleMap::iterator it = graph.triangles.begin(); it != graph.triangles.end(); ++it)
    {
      for (int i = 0; i < 3; i++)
      {
        PointTriangleMap::iterator pit = map.find(it->second.idx[i]);
        if (pit == map.end())
        {
          IdSet set;
          set.insert(it->first);
          map.insert(PointTriangleMap::value_type(it->second.idx[i], set));

          continue;
        }

        pit->second.insert(it->first);
      }
    }
  }
}