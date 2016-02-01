#pragma once

#include <opencv2/core/core.hpp>

#include <vector>
#include <map>
#include <set>

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
  typedef std::pair<cv::Point2f, uchar> PointValue;

  typedef std::vector<cv::Point2f> PointList;
  typedef std::vector<PointValue> PointValueList;
  typedef std::vector<PointValueList> PointValueListGroup;
  typedef std::pair<int, int> Edge;
  typedef std::pair<Edge, uchar> EdgeValue;
  typedef std::map<int, cv::Point2f> IdPointMap;
  typedef std::vector<EdgeValue> EdgeValueList;
  typedef std::map<int, int> PointEdgesCountMap;
  typedef std::map<int, Triangle> TriangleMap;
  typedef std::pair<int, uchar> NeighbourValue;
  typedef std::vector<NeighbourValue> NeighbourValueList;
  typedef std::map<int, NeighbourValueList> NeighbourMap;
  typedef std::multimap<double, int> AnglePointMap;

  typedef std::map<float, int> FloatIdMap;
  typedef std::map<float, FloatIdMap> FloatFloatIdMap;

  typedef std::set<int> IdSet;
  typedef std::map<int, IdSet> IdIdMap;

  typedef std::pair<int, uchar> PointIdValue;
  typedef std::multimap<double, PointIdValue> AnglePointIdValueMap;
  typedef std::set<int> PointSet;

  typedef std::map<int, IdSet> PointTriangleMap;


  typedef struct _graph
  {
    IdPointMap points;
    EdgeValueList edges;
    TriangleMap triangles;
  } Graph;

  class BoundingBox
  {
  public:
    BoundingBox();
    BoundingBox(PointList& points);
    BoundingBox(Graph& graph, PointSet& points);
    BoundingBox(cv::Point2f min, cv::Point2f max);

    cv::Point2f min() const;
    cv::Point2f max() const;

    float width() const;
    float height() const;

    cv::Point2f center() const;

  private:
    cv::Point2f m_min;
    cv::Point2f m_max;
  };

  void neighbourMapFromEdges(EdgeValueList& edges, NeighbourMap& neighbourMap);

  void removeNeighbourFromList(NeighbourValueList& list, int id);
  void removeNeighbourMapPoint(NeighbourMap& neighbourMap, int pointId, int& endpoint1, int& endpoint2, uchar& value);

  void makeFloatFloatIdMap(Graph& graph, FloatFloatIdMap& map);
  void makePointTriangleMap(Graph& graph, PointTriangleMap& map);
}