#pragma once

#include <opencv2/core/core.hpp>

#include <vector>
#include <map>

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

  typedef Array<bool> BoolArray;
  typedef Array<int> IntArray;
  typedef std::pair<cv::Point, uchar> PointValue;

  typedef std::vector<cv::Point> PointList;
  typedef std::vector<PointValue> PointValueList;
  typedef std::vector<PointValueList> PointValueListGroup;
  typedef std::pair<int, int> Edge;
  typedef std::pair<Edge, uchar> EdgeValue;
  typedef std::map<int, cv::Point2f> IdPointMap;
  typedef std::vector<EdgeValue> EdgeValueList;
  typedef std::map<int, int> PointEdgesCountMap;

  typedef struct _graph
  {
    IdPointMap points;
    EdgeValueList edges;
  } Graph;

}