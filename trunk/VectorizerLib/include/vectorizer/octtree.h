#pragma once

#include <osg/Vec3f>
#include <osg/BoundingBox>

#include <vector>
#include <memory>

namespace helper
{
  template<typename T>
  class OctTreeNodeData
  {
  public:
    OctTreeNodeData() {}

    OctTreeNodeData(osg::Vec3f point, T data)
      : m_point(point)
      , m_data(data)
    {

    }

    osg::Vec3f point() const
    {
      return m_point;
    }

    T data() const
    {
      return m_data;
    }

  private:
    osg::Vec3f m_point;
    T m_data;
  };

  template<typename T>
  class OctTreeNode
  {
  public:
    typedef std::vector<OctTreeNodeData<T>> Data;
    typedef std::pair<OctTreeNodeData<T>, OctTreeNodeData<T>> Arc;
    typedef std::pair<T, T> ArcData;
    typedef std::vector<Arc> ArcList;
    typedef std::set<ArcData> ArcDataSet;
    typedef void(QuadTreeNodeTraverseBlock)(OctTreeNode *node);

    OctTreeNode(const osg::BoundingBox& boundary, int capacity)
      : m_northWestFront(nullptr)
      , m_northEastFront(nullptr)
      , m_southWestFront(nullptr)
      , m_southEastFront(nullptr)
      , m_northWestBack(nullptr)
      , m_northEastBack(nullptr)
      , m_southWestBack(nullptr)
      , m_southEastBack(nullptr)
      , m_boundary(boundary)
      , m_overallBoundary(boundary)
      , m_capacity(capacity)
    {
    }

    OctTreeNode(const osg::BoundingBox& boundary, const osg::BoundingBox& overallBoundary, int capacity)
      : m_northWestFront(nullptr)
      , m_northEastFront(nullptr)
      , m_southWestFront(nullptr)
      , m_southEastFront(nullptr)
      , m_northWestBack(nullptr)
      , m_northEastBack(nullptr)
      , m_southWestBack(nullptr)
      , m_southEastBack(nullptr)
      , m_boundary(boundary)
      , m_overallBoundary(overallBoundary)
      , m_capacity(capacity)
    {
    }

    ~OctTreeNode()
    {
      if (m_northWestFront != nullptr) delete m_northWestFront;
      if (m_northEastFront != nullptr) delete m_northEastFront;
      if (m_southWestFront != nullptr) delete m_southWestFront;
      if (m_southEastFront != nullptr) delete m_southEastFront;
      if (m_northWestBack != nullptr) delete m_northWestBack;
      if (m_northEastBack != nullptr) delete m_northEastBack;
      if (m_southWestBack != nullptr) delete m_southWestBack;
      if (m_southEastBack != nullptr) delete m_southEastBack;
    }

    void gatherDataWithinBoundary(const osg::BoundingBox& boundary, Data& results)
    {
      // If range is not contained in the node's boundingBox then bail
      if (!boundingBoxIntersectsBoundingBox(m_boundary, boundary)) {
        return;
      }

      for (typename Data::iterator it = m_data.begin(); it != m_data.end(); ++it)
      {
        // Gather points contained in range
        if (boundingBoxContainsData(boundary, *it))
          results.push_back(*it);
      }

      // Bail if node is leaf
      if (m_northWestFront == nullptr)
        return;

      // Otherwise traverse down the tree
      m_northWestFront->gatherDataWithinBoundary(boundary, results);
      m_northEastFront->gatherDataWithinBoundary(boundary, results);
      m_southWestFront->gatherDataWithinBoundary(boundary, results);
      m_southEastFront->gatherDataWithinBoundary(boundary, results);
      m_northWestBack->gatherDataWithinBoundary(boundary, results);
      m_northEastBack->gatherDataWithinBoundary(boundary, results);
      m_southWestBack->gatherDataWithinBoundary(boundary, results);
      m_southEastBack->gatherDataWithinBoundary(boundary, results);
    }

    void gatherArcDataWithinBoundary(const osg::BoundingBox& boundary, ArcDataSet& results)
    {
      if (!boundingBoxIntersectsBoundingBox(m_boundary, boundary)) {
        return;
      }

      if (m_arcs.size() > 0)
      {
        for (typename ArcList::iterator it = m_arcs.begin(); it != m_arcs.end(); ++it)
        {
          ArcData arc;
          arc.first = it->first.data();
          arc.second = it->second.data();

          if (results.count(arc) == 0)
            results.insert(arc);
        }
      }
        

      if (m_northWestFront == nullptr)
        return;

      m_northWestFront->gatherArcDataWithinBoundary(boundary, results);
      m_northEastFront->gatherArcDataWithinBoundary(boundary, results);
      m_southWestFront->gatherArcDataWithinBoundary(boundary, results);
      m_southEastFront->gatherArcDataWithinBoundary(boundary, results);
      m_northWestBack->gatherArcDataWithinBoundary(boundary, results);
      m_northEastBack->gatherArcDataWithinBoundary(boundary, results);
      m_southWestBack->gatherArcDataWithinBoundary(boundary, results);
      m_southEastBack->gatherArcDataWithinBoundary(boundary, results);
    }

    void traverse(QuadTreeNodeTraverseBlock block)
    {
      block(this);

      if (m_northWestFront == nullptr)
        return;

      m_northWestFront->traverse(block);
      m_northEastFront->traverse(block);
      m_southWestFront->traverse(block);
      m_southEastFront->traverse(block);
      m_northWestBack->traverse(block);
      m_northEastBack->traverse(block);
      m_southWestBack->traverse(block);
      m_southEastBack->traverse(block);
    }
    
    bool insert(const OctTreeNodeData<T>& data)
    {
      // Return if our coordinate is not in the boundingBox
      if (!boundingBoxContainsData(m_boundary, data))
        return false;

      // Add the coordinate to the points array.
      if (int(m_data.size()) < m_capacity)
      {
        m_data.push_back(data);
        return true;
      }

      // Check to see if the current node is a leaf, if it is, split.
      if (m_northWestFront == nullptr)
        subdivide();

      // Traverse the tree
      if (m_northWestFront->insert(data)) return true;
      if (m_northEastFront->insert(data)) return true;
      if (m_southWestFront->insert(data)) return true;
      if (m_southEastFront->insert(data)) return true;
      if (m_northWestBack->insert(data)) return true;
      if (m_northEastBack->insert(data)) return true;
      if (m_southWestBack->insert(data)) return true;
      if (m_southEastBack->insert(data)) return true;

      // Default. Was unable to add the node.
      return false;
    }

    bool insertCenteredSphereArc(osg::Vec3f p1, T p1Data, osg::Vec3f p2, T p2Data, float radius, std::shared_ptr<osg::BoundingBox> bb = std::shared_ptr<osg::BoundingBox>())
    {
      if (!bb)
      {
        bb.reset(new osg::BoundingBox());
        getCenteredSphereArcBoundingBox(p1, p2, radius, *bb.get());
      }

      if (!boundingBoxIntersectsBoundingBox(m_boundary, *bb.get()))
        return false;

      if (m_northWestFront == nullptr)
      {
        Arc arc;
        arc.first = OctTreeNodeData<T>(p1, p1Data);
        arc.second = OctTreeNodeData<T>(p2, p2Data);

        m_arcs.push_back(arc);
        return true;
      }

      bool bSuccess = false;
      if (m_northWestFront->insertCenteredSphereArc(p1 ,p1Data, p2, p2Data, radius, bb)) bSuccess = true;
      if (m_northEastFront->insertCenteredSphereArc(p1, p1Data, p2, p2Data, radius, bb)) bSuccess = true;
      if (m_southWestFront->insertCenteredSphereArc(p1, p1Data, p2, p2Data, radius, bb)) bSuccess = true;
      if (m_southEastFront->insertCenteredSphereArc(p1, p1Data, p2, p2Data, radius, bb)) bSuccess = true;
      if (m_northWestBack->insertCenteredSphereArc(p1, p1Data, p2, p2Data, radius, bb)) bSuccess = true;
      if (m_northEastBack->insertCenteredSphereArc(p1, p1Data, p2, p2Data, radius, bb)) bSuccess = true;
      if (m_southWestBack->insertCenteredSphereArc(p1, p1Data, p2, p2Data, radius, bb)) bSuccess = true;
      if (m_southEastBack->insertCenteredSphereArc(p1, p1Data, p2, p2Data, radius, bb)) bSuccess = true;

      return bSuccess;
    }

    void subdivide()
    {
      osg::BoundingBox box = m_boundary;

      // Spit the quadrant into four equal parts.
      float xMid = (box.xMax() + box.xMin()) / 2.0f;
      float yMid = (box.yMax() + box.yMin()) / 2.0f;
      float zMid = (box.zMax() + box.zMin()) / 2.0f;

      // Create the north west front bounding box.
      osg::BoundingBox northWestFrontbb(osg::Vec3f(box.xMin(), box.yMin(), box.zMin()), osg::Vec3f(xMid, yMid, zMid));
      m_northWestFront = new OctTreeNode(northWestFrontbb, m_overallBoundary, m_capacity);

      // Create the north east front bounding box.
      osg::BoundingBox northEastFrontbb(osg::Vec3f(xMid, box.yMin(), box.zMin()), osg::Vec3f(box.xMax(), yMid, zMid));
      m_northEastFront = new OctTreeNode(northEastFrontbb, m_overallBoundary, m_capacity);

      // Create the south west front bounding box.
      osg::BoundingBox southWestFrontbb(osg::Vec3f(box.xMin(), yMid, box.zMin()), osg::Vec3f(xMid, box.yMax(), zMid));
      m_southWestFront = new OctTreeNode(southWestFrontbb, m_overallBoundary, m_capacity);

      // Create the south east front bounding box.
      osg::BoundingBox southEastFrontbb(osg::Vec3f(xMid, yMid, box.zMin()), osg::Vec3f(box.xMax(), box.yMax(), zMid));
      m_southEastFront = new OctTreeNode(southEastFrontbb, m_overallBoundary, m_capacity);

      // Create the north west back bounding box.
      osg::BoundingBox northWestBackbb(osg::Vec3f(box.xMin(), box.yMin(), zMid), osg::Vec3f(xMid, yMid, box.zMax()));
      m_northWestBack = new OctTreeNode(northWestBackbb, m_overallBoundary, m_capacity);

      // Create the north east back bounding box.
      osg::BoundingBox northEastBackbb(osg::Vec3f(xMid, box.yMin(), zMid), osg::Vec3f(box.xMax(), yMid, box.zMax()));
      m_northEastBack = new OctTreeNode(northEastBackbb, m_overallBoundary, m_capacity);

      // Create the south west back bounding box.
      osg::BoundingBox southWestBackbb(osg::Vec3f(box.xMin(), yMid, zMid), osg::Vec3f(xMid, box.yMax(), box.zMax()));
      m_southWestBack = new OctTreeNode(southWestBackbb, m_overallBoundary, m_capacity);

      // Create the south east back bounding box.
      osg::BoundingBox southEastBackbb(osg::Vec3f(xMid, yMid, zMid), osg::Vec3f(box.xMax(), box.yMax(), box.zMax()));
      m_southEastBack = new OctTreeNode(southEastBackbb, m_overallBoundary, m_capacity);
    }

  protected:
    bool boundingBoxContainsData(const osg::BoundingBox& boundary, const OctTreeNodeData<T>& data)
    {
      bool containsX = boundary.xMin() <= data.point().x() && data.point().x() <= boundary.xMax();
      bool containsY = boundary.yMin() <= data.point().y() && data.point().y() <= boundary.yMax();
      bool containsZ = boundary.zMin() <= data.point().z() && data.point().z() <= boundary.zMax();

      return containsX && containsY && containsZ;
    }

    bool boundingBoxIntersectsBoundingBox(const osg::BoundingBox& boundary, const osg::BoundingBox& test)
    {
      return (
        boundary.xMin() <= test.xMax() && boundary.xMax() >= test.xMin() &&
        boundary.yMin() <= test.yMax() && boundary.yMax() >= test.yMin() && 
        boundary.zMin() <= test.zMax() && boundary.zMax() >= test.zMin());
    }

    // Note: arc should be shorter than a quarter of the perimeter
    void getCenteredSphereArcBoundingBox(const osg::Vec3f& p1, const osg::Vec3f& p2, float radius, osg::BoundingBox& bb)
    {
      float xMid = (m_overallBoundary.xMax() + m_overallBoundary.xMin()) / 2.0f;
      float yMid = (m_overallBoundary.yMax() + m_overallBoundary.yMin()) / 2.0f;
      float zMid = (m_overallBoundary.zMax() + m_overallBoundary.zMin()) / 2.0f;

      osg::Vec3f pmin(std::min<float>(p1.x(), p2.x()), std::min<float>(p1.y(), p2.y()), std::min<float>(p1.z(), p2.z()));
      osg::Vec3f pmax(std::max<float>(p1.x(), p2.x()), std::max<float>(p1.y(), p2.y()), std::max<float>(p1.z(), p2.z()));

      bb = osg::BoundingBox(pmin, pmax);

      osg::Vec3 up = p1 ^ p2;

      osg::Vec3f v_yz = up ^ osg::Vec3f(1.0f, 0.0f, 0.0f);
      osg::Vec3f v_xz = up ^ osg::Vec3f(0.0f, 1.0f, 0.0f);
      osg::Vec3f v_xy = up ^ osg::Vec3f(0.0f, 0.0f, 1.0f);

      v_yz.normalize();
      v_xz.normalize();
      v_xy.normalize();

      v_yz *= radius;
      v_xz *= radius;
      v_xy *= radius;

      if ((p1.x() < xMid && p2.x() > xMid) || (p2.x() < xMid && p1.x() > xMid) )
      {
        if (p1.y() >= yMid || p2.y() >= yMid)
          bb.expandBy(bb._max.x(), std::max<float>(v_xy.y(), -v_xy.y()), bb._max.z());
        if (p1.z() >= zMid || p2.z() >= zMid)
          bb.expandBy(bb._max.x(), bb._max.y(), std::max<float>(v_xy.z(), -v_xy.z()));

        if (p1.y() <= yMid || p2.y() <= yMid)
          bb.expandBy(bb._min.x(), std::min<float>(v_xy.y(), -v_xy.y()), bb._min.z());
        if (p1.z() <= zMid || p2.z() <= zMid)
          bb.expandBy(bb._min.x(), bb._min.y(), std::min<float>(v_xy.z(), -v_xy.z()));
      }

      if ((p1.y() < yMid && p2.y() > yMid) || (p2.y() < yMid && p1.y() > yMid))
      {
        if (p1.x() >= xMid || p2.x() >= xMid)
          bb.expandBy(std::max<float>(v_xz.x(), -v_xz.x()), bb._max.y(), bb._max.z());
        if (p1.z() >= zMid || p2.z() >= zMid)
          bb.expandBy(bb._max.x(), bb._max.y(), std::max<float>(v_xz.z(), -v_xz.z()));

        if (p1.x() <= xMid || p2.x() <= xMid)
          bb.expandBy(std::min<float>(v_xz.x(), -v_xz.x()), bb._min.y(), bb._min.z());
        if (p1.z() <= zMid || p2.z() <= zMid)
          bb.expandBy(bb._min.x(), bb._min.y(), std::min<float>(v_xz.z(), -v_xz.z()));
      }

      if ((p1.z() < zMid && p2.z() > zMid) || (p2.z() < zMid && p1.z() > zMid))
      {
        if (p1.x() >= xMid || p2.x() >= xMid)
          bb.expandBy(std::max<float>(v_xy.x(), -v_xy.x()), bb._max.y(), bb._max.z());
        if (p1.y() >= yMid || p2.y() >= yMid)
          bb.expandBy(bb._max.x(), std::max<float>(v_xy.y(), -v_xy.y()), bb._max.z());

        if (p1.x() <= xMid || p2.x() <= xMid)
          bb.expandBy(std::min<float>(v_xy.x(), -v_xy.x()), bb._min.y(), bb._min.z());
        if (p1.y() <= yMid || p2.y() <= yMid)
          bb.expandBy(bb._min.x(), std::min<float>(v_xy.y(), -v_xy.y()), bb._min.z());
      }
    }

    OctTreeNode* m_northWestFront;
    OctTreeNode* m_northEastFront;
    OctTreeNode* m_southWestFront;
    OctTreeNode* m_southEastFront;
    OctTreeNode* m_northWestBack;
    OctTreeNode* m_northEastBack;
    OctTreeNode* m_southWestBack;
    OctTreeNode* m_southEastBack;

    Data m_data;
    ArcList m_arcs;
    osg::BoundingBox m_boundary;
    osg::BoundingBox m_overallBoundary;

    int m_capacity;
  };

}