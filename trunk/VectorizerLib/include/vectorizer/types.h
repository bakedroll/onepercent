#pragma once

#include <opencv2/core/core.hpp>

#include <osg/Vec3f>

#include <vector>
#include <map>
#include <set>

namespace helper
{
  template<typename T>
  class Matrix
  {
  public:
    Matrix(int cols, int rows, T initialValue)
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

    ~Matrix()
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

  struct Triangle
  {
    Triangle() {}
    Triangle(int p1, int p2, int p3)
    {
      idx[0] = p1;
      idx[1] = p2;
      idx[2] = p3;
    }

    int idx[3];
  };

  struct Quad
  {
    Quad(int p1, int p2, int p3, int p4)
    {
      idx[0] = p1;
      idx[1] = p2;
      idx[2] = p3;
      idx[3] = p4;
    }

    int idx[4];
  };

  struct Point3D
  {
    Point3D() : originId(-1) {}
    Point3D(float x, float y, float z, int oid = -1)
      : originId(oid)
    {
      value[0] = x;
      value[1] = y;
      value[2] = z;
    }

    float value[3];
    int originId;
  };

  typedef Matrix<bool> BoolMatrix;
  typedef Matrix<int> IntMatrix;
  typedef std::pair<cv::Point2i, uchar> PointValue;

  typedef std::vector<cv::Point2i> PointListi;
  typedef std::vector<cv::Point2f> PointListf;
  typedef std::vector<PointValue> PointValueList;
  typedef std::vector<PointValueList> PointValueListGroup;
  typedef std::pair<int, int> Edge;
  typedef std::vector<Edge> EdgeList;
  typedef std::vector<EdgeList> EdgeListList;
  typedef std::pair<Edge, uchar> EdgeValue;
  typedef std::map<int, cv::Point2f> IdPointMap;
  typedef std::map<int, Point3D> IdPoint3DMap;
  typedef std::vector<EdgeValue> EdgeValueList;
  typedef std::map<int, EdgeList> NeighbourEdgesMap;
  typedef std::map<int, int> IdMap;
  typedef std::vector<int> IdList;
  typedef std::map<int, Triangle> TriangleMap;
  typedef std::vector<Quad> QuadList;
  typedef std::map<int, QuadList> IdQuadListMap;
  typedef std::pair<int, uchar> NeighbourValue;
  typedef std::vector<NeighbourValue> NeighbourValueList;
  typedef std::map<int, NeighbourValueList> NeighbourMap;
  typedef std::multimap<double, int> AnglePointMap;

  typedef std::map<float, int> FloatIdMap;
  typedef std::map<float, FloatIdMap> FloatFloatIdMap;

  typedef std::pair<int, uchar> PointIdValue;
  typedef std::multimap<double, PointIdValue> AnglePointIdValueMap;
  typedef std::set<int> IdSet;
  typedef std::map<int, IdSet> IdIdSetMap;

  typedef std::map<int, IdSet> PointTriangleMap;

  typedef std::map<std::pair<int, int>, QuadList> BoundaryNodalMap;

  template<typename T>
  class BoundingBox
  {
  public:
    BoundingBox()
    {
      reset();
    }

    BoundingBox(std::vector<cv::Point_<T>>& points)
    {
      reset();
      for (typename std::vector<cv::Point_<T>>::iterator it = points.begin(); it != points.end(); ++it)
        expand(*it);
    }

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

    T width(bool addone = true) const
    {
      T w = m_max.x - m_min.x;
      if (addone)
        w += static_cast<T>(1);

      return w;
    }

    T height(bool addone = true) const
    {
      T h = m_max.y - m_min.y;
      if (addone)
        h += static_cast<T>(1);

      return h;
    }

    cv::Point_<T> center() const
    {
      return m_min + ((m_max - m_min) / static_cast<T>(2));
    }

    void expand(const cv::Point_<T>& point)
    {
      m_min.x = std::min<T>(m_min.x, point.x);
      m_min.y = std::min<T>(m_min.y, point.y);
      m_max.x = std::max<T>(m_max.x, point.x);
      m_max.y = std::max<T>(m_max.y, point.y);
    }

    void expand(const BoundingBox<T> bb)
    {
      m_min.x = std::min<T>(m_min.x, bb.m_min.x);
      m_min.y = std::min<T>(m_min.y, bb.m_min.y);
      m_max.x = std::max<T>(m_max.x, bb.m_max.x);
      m_max.y = std::max<T>(m_max.y, bb.m_max.y);
    }

    void scale(float x, float y)
    {
      m_min.x *= x;
      m_max.x *= x;
      m_min.y *= y;
      m_max.y *= y;
    }

    void shift(T x, T y)
    {
      m_min += cv::Point_<T>(x, y);
      m_max += cv::Point_<T>(x, y);
    }

  private:
    void reset()
    {
      m_min = cv::Point_<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
      m_max = cv::Point_<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::min());
    }

    cv::Point_<T> m_min;
    cv::Point_<T> m_max;
  };

  struct Graph
  {
    IdPointMap points;
    EdgeValueList edges;
    TriangleMap triangles;
    BoundingBox<float> boundary;
  };

  struct Border
  {
    int boundarySegmentId;
    int countryId;
    int neighbourId;
    IdSet points;
    IdList nextBorderIds;
    std::pair<int, int> startInnerOuterPointId;
    std::pair<int, int> endInnerOuterPointId;
    bool bIsCycle;
  };

  typedef std::vector<Border> BorderList;
  typedef std::map<int, Border> IdBorderMap;
  typedef std::map<int, IdList> IdBorderIdListMap;

  struct Cycle
  {
    NeighbourEdgesMap edges;
    TriangleMap trianlges;
    BoundingBox<float> boundingbox;
  };

  typedef std::map<int, Cycle> CyclesMap;

  struct SphericalMesh
  {
    IdPoint3DMap points;
    EdgeValueList edges;
    QuadList quads;
    int nInnerPoints;
  };

  struct CountryData
  {
    int id;
    std::string name;
  };

  struct CountryRow
  {
    CountryData data;
    IdSet cycleIds;
    IdSet additionalNeighbourIds;
  };

  typedef std::vector<CountryRow> CountriesTable;

  struct Country
  {
    CountryData data;
    BoundingBox<float> boundingbox;
    TriangleMap triangles;
    IdSet neighbours;
    NeighbourEdgesMap neighbourEdges;
    IdBorderIdListMap borders;
  };

  typedef std::map<int, Country> CountriesMap;

  struct BoundariesMeshData
  {
    IdPoint3DMap points;
    IdQuadListMap boundarySegments;
    IdQuadListMap boundaryNodalsFull;
    BoundaryNodalMap boundaryNodals;
    IdBorderMap borders;
  };

  struct WorldData
  {
    CountriesMap countries;
    BoundariesMeshData boundariesMeshData;
  };

  void neighbourMapFromEdges(EdgeValueList& edges, NeighbourMap& neighbourMap);
  void neighbourMapFromEdges(NeighbourEdgesMap& edges, NeighbourMap& neighbourMap);
  void neighbourMapFromEdges(EdgeList& edges, NeighbourMap& neighbourMap);

  void removeNeighbourFromList(NeighbourValueList& list, int id);
  bool removeNeighbourMapPoint(NeighbourMap& neighbourMap, int pointId, int& endpoint1, int& endpoint2, uchar& value);

  void makeAnglePointMap(IdPointMap& graph, NeighbourValueList& neighbours, int originId, int p1Id, AnglePointIdValueMap& angles);
  void makePointTriangleMap(Graph& graph, PointTriangleMap& map);

  void makePointList(IdPointMap& points, IdSet& ids, PointListf& result);

  void removeUnusedPoints(IdPoint3DMap& points, EdgeValueList& edges, IdPoint3DMap& results);

  void findSegments(EdgeList& edges, EdgeListList& results);

  void makeCartesianPoint(cv::Point2f p, float width, float height, osg::Vec3f& p3d, float radius, float shift);
  void makeCartesianPoints(Graph& graph, IdPoint3DMap& points, float radius, float shift);
  void makePolarPoints(IdPoint3DMap& input, IdPoint3DMap& output, int width, int height, float shift);
}