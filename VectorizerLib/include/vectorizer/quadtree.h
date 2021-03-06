#pragma once

#include "types.h"

#include <vector>

namespace helper
{
  template<typename T>
  class QuadTreeNodeData
  {
  public:
    QuadTreeNodeData(cv::Point2f point, T data)
      : m_point(point)
      , m_data(data)
    {
      
    }

    cv::Point2f point() const
    {
      return m_point;
    }

    T data() const
    {
      return m_data;
    }

  private:
    cv::Point2f m_point;
    T m_data;
  };

  template<typename T>
  class QuadTreeNode
  {
  public:
    typedef std::vector<QuadTreeNodeData<T>> Data;
    typedef void(QuadTreeNodeTraverseBlock)(QuadTreeNode *node);

    QuadTreeNode(BoundingBox<float> boundary, int capacity)
      : m_northWest(nullptr)
      , m_northEast(nullptr)
      , m_southWest(nullptr)
      , m_southEast(nullptr)
      , m_boundary(boundary)
      , m_capacity(capacity)
    {
    }

    ~QuadTreeNode()
    {
      if (m_northWest != nullptr) delete m_northWest;
      if (m_northEast != nullptr) delete m_northEast;
      if (m_southWest != nullptr) delete m_southWest;
      if (m_southEast != nullptr) delete m_southEast;
    }

    void gatherDataWithinBoundary(const BoundingBox<float>& boundary, Data& results)
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
      if (m_northWest == nullptr)
        return;

      // Otherwise traverse down the tree
      m_northWest->gatherDataWithinBoundary(boundary, results);
      m_northEast->gatherDataWithinBoundary(boundary, results);
      m_southWest->gatherDataWithinBoundary(boundary, results);
      m_southEast->gatherDataWithinBoundary(boundary, results);
    }

    void traverse(QuadTreeNodeTraverseBlock block)
    {
      block(this);

      if (m_northWest == nullptr)
        return;

      m_northWest->traverse(block);
      m_northEast->traverse(block);
      m_southWest->traverse(block);
      m_southEast->traverse(block);
    }

    bool insert(const QuadTreeNodeData<T>& data)
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
      if (m_northWest == nullptr)
        subdivide();

      // Traverse the tree
      if (m_northWest->insert(data)) return true;
      if (m_northEast->insert(data)) return true;
      if (m_southWest->insert(data)) return true;
      if (m_southEast->insert(data)) return true;

      // Default. Was unable to add the node.
      return false;
    }

    void subdivide()
    {
      BoundingBox<float> box = m_boundary;

      // Spit the quadrant into four equal parts.
      float xMid = (box.max().x + box.min().x) / 2.0f;
      float yMid = (box.max().y + box.min().y) / 2.0f;

      // Create the north west bounding box.
      BoundingBox<float> northWestbb(cv::Point2f(box.min().x, box.min().y), cv::Point2f(xMid, yMid));
      m_northWest = new QuadTreeNode(northWestbb, m_capacity);

      // Create the north east bounding box.
      BoundingBox<float> northEastbb(cv::Point2f(xMid, box.min().y), cv::Point2f(box.max().x, yMid));
      m_northEast = new QuadTreeNode(northEastbb, m_capacity);

      // Create the south west bounding box.
      BoundingBox<float> southWestbb(cv::Point2f(box.min().x, yMid), cv::Point2f(xMid, box.max().y));
      m_southWest = new QuadTreeNode(southWestbb, m_capacity);

      // Create the south east bounding box.
      BoundingBox<float> southEastbb(cv::Point2f(xMid, yMid), cv::Point2f(box.max().x, box.max().y));
      m_southEast = new QuadTreeNode(southEastbb, m_capacity);
    }

  protected:
    bool boundingBoxContainsData(const BoundingBox<float>& boundary, const QuadTreeNodeData<T>& data)
    {
      bool containsX = boundary.min().x <= data.point().x && data.point().x <= boundary.max().x;
      bool containsY = boundary.min().y <= data.point().y && data.point().y <= boundary.max().y;

      return containsX && containsY;
    }

    bool boundingBoxIntersectsBoundingBox(const BoundingBox<float>& boundary, const BoundingBox<float>& test)
    {
      return (boundary.min().x <= test.max().x && boundary.max().x >= test.min().x && boundary.min().y <= test.max().y && boundary.max().y >= test.min().y);
    }

    QuadTreeNode* m_northWest;
    QuadTreeNode* m_northEast;
    QuadTreeNode* m_southWest;
    QuadTreeNode* m_southEast;

    Data m_data;
    BoundingBox<float> m_boundary;

    int m_capacity;
  };

}