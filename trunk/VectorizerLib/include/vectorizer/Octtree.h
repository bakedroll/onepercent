#pragma once

#include <osg/Vec3f>
#include <osg/BoundingBox>

#include <vector>


namespace helper
{
  template<typename T>
  class OctTreeNodeData
  {
  public:
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
    typedef void(QuadTreeNodeTraverseBlock)(OctTreeNode *node);

    OctTreeNode(osg::BoundingBox boundary, int capacity)
      : m_northWestFront(nullptr)
      , m_northEastFront(nullptr)
      , m_southWestFront(nullptr)
      , m_southEastFront(nullptr)
      , m_northWestBack(nullptr)
      , m_northEastBack(nullptr)
      , m_southWestBack(nullptr)
      , m_southEastBack(nullptr)
      , m_boundary(boundary)
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

    void subdivide()
    {
      osg::BoundingBox box = m_boundary;

      // Spit the quadrant into four equal parts.
      float xMid = (box.xMax() + box.xMin()) / 2.0f;
      float yMid = (box.yMax() + box.yMin()) / 2.0f;
      float zMid = (box.zMax() + box.zMin()) / 2.0f;

      // Create the north west front bounding box.
      osg::BoundingBox northWestFrontbb(osg::Vec3f(box.xMin(), box.yMin(), box.zMin()), osg::Vec3f(xMid, yMid, zMid));
      m_northWestFront = new OctTreeNode(northWestFrontbb, m_capacity);

      // Create the north east front bounding box.
      osg::BoundingBox northEastFrontbb(osg::Vec3f(xMid, box.yMin(), box.zMin()), osg::Vec3f(box.xMax(), yMid, zMid));
      m_northEastFront = new OctTreeNode(northEastFrontbb, m_capacity);

      // Create the south west front bounding box.
      osg::BoundingBox southWestFrontbb(osg::Vec3f(box.xMin(), yMid, box.zMin()), osg::Vec3f(xMid, box.yMax(), zMid));
      m_southWestFront = new OctTreeNode(southWestFrontbb, m_capacity);

      // Create the south east front bounding box.
      osg::BoundingBox southEastFrontbb(osg::Vec3f(xMid, yMid, box.zMin()), osg::Vec3f(box.xMax(), box.yMax(), zMid));
      m_southEastFront = new OctTreeNode(southEastFrontbb, m_capacity);

      // Create the north west back bounding box.
      osg::BoundingBox northWestBackbb(osg::Vec3f(box.xMin(), box.yMin(), zMid), osg::Vec3f(xMid, yMid, box.zMax()));
      m_northWestFront = new OctTreeNode(northWestBackbb, m_capacity);

      // Create the north east back bounding box.
      osg::BoundingBox northEastBackbb(osg::Vec3f(xMid, box.yMin(), zMid), osg::Vec3f(box.xMax(), yMid, box.zMax()));
      m_northEastFront = new OctTreeNode(northEastBackbb, m_capacity);

      // Create the south west back bounding box.
      osg::BoundingBox southWestBackbb(osg::Vec3f(box.xMin(), yMid, zMid), osg::Vec3f(xMid, box.yMax(), box.zMax()));
      m_southWestFront = new OctTreeNode(southWestBackbb, m_capacity);

      // Create the south east back bounding box.
      osg::BoundingBox southEastBackbb(osg::Vec3f(xMid, yMid, zMid), osg::Vec3f(box.xMax(), box.yMax(), box.zMax()));
      m_southEastFront = new OctTreeNode(southEastBackbb, m_capacity);
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

    OctTreeNode* m_northWestFront;
    OctTreeNode* m_northEastFront;
    OctTreeNode* m_southWestFront;
    OctTreeNode* m_southEastFront;
    OctTreeNode* m_northWestBack;
    OctTreeNode* m_northEastBack;
    OctTreeNode* m_southWestBack;
    OctTreeNode* m_southEastBack;

    Data m_data;
    osg::BoundingBox m_boundary;

    int m_capacity;
  };

}