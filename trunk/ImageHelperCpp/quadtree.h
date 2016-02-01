#pragma once

#include "types.h"

#include <vector>

namespace helper
{
  class QuadTreeNodeData
  {
  public:
    QuadTreeNodeData(cv::Point2f point, int data)
      : m_point(point)
      , m_data(data)
    {
      
    }

    cv::Point2f point() const
    {
      return m_point;
    }

    int data() const
    {
      return m_data;
    }

  private:
    cv::Point2f m_point;
    int m_data;
  };

  class QuadTreeNode
  {
  public:
    typedef void(QuadTreeNodeTraverseBlock)(QuadTreeNode *node);
    typedef std::vector<QuadTreeNodeData> QuadTreeNodeDataList;

    QuadTreeNode(BoundingBox boundary, int capacity);
    ~QuadTreeNode();

    void gatherDataWithinBoundary(const BoundingBox& boundary, QuadTreeNodeDataList& results);
    void traverse(QuadTreeNodeTraverseBlock block);
    bool insert(const QuadTreeNodeData& data);
    void subdivide();

  protected:
    bool boundingBoxContainsData(const BoundingBox& boundary, const QuadTreeNodeData& data);
    bool boundingBoxIntersectsBoundingBox(const BoundingBox& boundary, const BoundingBox& test);

    QuadTreeNode *m_northWest;
    QuadTreeNode *m_northEast;
    QuadTreeNode *m_southWest;
    QuadTreeNode *m_southEast;

    QuadTreeNodeDataList m_data;
    BoundingBox m_boundary;

    int m_capacity;
  };

}