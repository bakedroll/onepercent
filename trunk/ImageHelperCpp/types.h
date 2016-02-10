#pragma once

#include <opencv2/core/core.hpp>

#include <vector>
#include <map>
#include <set>
#include <osg/Vec3f>

namespace helper
{
  template<typename T>
  class Array
  {
  public:
    Array(int cols, int rows, T initialValue)
      : m_cols(cols)
      , m_rows(rows)
    {
      m_data = new T*[m_cols];
      for (int x = 0; x < m_cols; x++)
      {
        m_data[x] = new T[m_rows];

        for (int y = 0; y < m_rows; y++)
        {
          m_data[x][y] = initialValue;
        }
      }
    }

    ~Array()
    {
      for (int x = 0; x < m_cols; x++)
        delete[] m_data[x];
      delete[] m_data;
    }

    int cols()
    {
      return m_cols;
    }

    int rows()
    {
      return m_rows;
    }

    T get(int col, int row)
    {
      return m_data[col][row];
    }

    void set(int col, int row, T value)
    {
      m_data[col][row] = value;
    }

  private:
    T** m_data;
    int m_cols;
    int m_rows;
  };

  typedef struct _triangle
  {
    int idx[3];
  } Triangle;

  typedef Array<bool> BoolArray;
  typedef Array<int> IntArray;
  typedef std::pair<cv::Point2i, uchar> PointValue;

  typedef std::vector<cv::Point2i> PointListi;
  typedef std::vector<cv::Point2f> PointListf;
  typedef std::vector<PointValue> PointValueList;
  typedef std::vector<PointValueList> PointValueListGroup;
  typedef std::pair<int, int> Edge;
  typedef std::pair<Edge, uchar> EdgeValue;
  typedef std::map<int, cv::Point2f> IdPointMap;
  typedef std::map<int, osg::Vec3f> IdPoint3DMap;
  typedef std::vector<EdgeValue> EdgeValueList;
  typedef std::map<int, int> IdMap;
  typedef std::map<int, Triangle> TriangleMap;
  typedef std::pair<int, uchar> NeighbourValue;
  typedef std::vector<NeighbourValue> NeighbourValueList;
  typedef std::map<int, NeighbourValueList> NeighbourMap;
  typedef std::multimap<double, int> AnglePointMap;

  typedef std::map<float, int> FloatIdMap;
  typedef std::map<float, FloatIdMap> FloatFloatIdMap;


  typedef std::pair<int, uchar> PointIdValue;
  typedef std::multimap<double, PointIdValue> AnglePointIdValueMap;
  typedef std::set<int> IdSet;
  typedef std::map<int, IdSet> IdIdMap;

  typedef std::map<int, IdSet> PointTriangleMap;

  template<typename T>
  class BoundingBox
  {
  public:
    BoundingBox()
    {
      
    }

    BoundingBox(std::vector<cv::Point_<T>>& points)
    {
      reset();
      for (typename std::vector<cv::Point_<T>>::iterator it = points.begin(); it != points.end(); ++it)
        expand(*it);
    }
    /*
    BoundingBox(std::map<int, cv::Point_<T> >& points, IdSet ids)
    {
      reset();
      for (typename std::map<int, cv::Point_<T>>::iterator it = ids.begin(); it != ids.end(); ++it)
        expand(points.find(*it)->second);
    }
    */
    BoundingBox(cv::Point_<T> min, cv::Point_<T> max)
      : m_min(min)
      , m_max(max)
    {

    }

    cv::Point_<T> min() const
    {
      return m_min;
    }

    cv::Point_<T> max() const
    {
      return m_max;
    }

    T width() const
    {
      return m_max.x - m_min.x + static_cast<T>(1);
    }

    T height() const
    {
      return m_max.y - m_min.y + static_cast<T>(1);
    }

    cv::Point_<T> center() const
    {
      return m_min + m_max / static_cast<T>(2);
    }

  private:
    void reset()
    {
      m_min = cv::Point_<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
      m_max = cv::Point_<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::min());
    }

    void expand(const cv::Point_<T>& point)
    {
      m_min.x = std::min<T>(m_min.x, point.x);
      m_min.y = std::min<T>(m_min.y, point.y);
      m_max.x = std::max<T>(m_max.x, point.x);
      m_max.y = std::max<T>(m_max.y, point.y);
    }

    cv::Point_<T> m_min;
    cv::Point_<T> m_max;
  };

  typedef struct _graph
  {
    IdPointMap points;
    EdgeValueList edges;
    TriangleMap triangles;
    BoundingBox<float> boundary;
  } Graph;

  typedef struct _sphericalMesh
  {
    IdPoint3DMap points;
    EdgeValueList edges;
  } SphericalMesh;

  void neighbourMapFromEdges(EdgeValueList& edges, NeighbourMap& neighbourMap);

  void removeNeighbourFromList(NeighbourValueList& list, int id);
  bool removeNeighbourMapPoint(NeighbourMap& neighbourMap, int pointId, int& endpoint1, int& endpoint2, uchar& value);

  void makeAnglePointMap(Graph& graph, NeighbourValueList& neighbours, int originId, int p1Id, AnglePointIdValueMap& angles);
  void makePointTriangleMap(Graph& graph, PointTriangleMap& map);

  void makePointList(IdPointMap& points, IdSet& ids, PointListf& result);

  void removeUnusedPoints(IdPoint3DMap& points, EdgeValueList& edges, IdPoint3DMap& results);
}