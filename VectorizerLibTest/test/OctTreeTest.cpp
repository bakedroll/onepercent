#include <gtest/gtest.h>

#include <vectorizer/octtree.h>

TEST(OctTreeTest, FindPoints)
{
  helper::OctTreeNode<int> tree(
    osg::BoundingBox(
      osg::Vec3f(-10.0f, -10.0f, -10.0f),
      osg::Vec3f(10.0f, 10.0f, 10.0f)), 10);

  tree.insert(helper::OctTreeNodeData<int>(osg::Vec3f(-5.0f, -5.0f, 1.0f), 0));
  tree.insert(helper::OctTreeNodeData<int>(osg::Vec3f(5.0f, 5.0f, 1.0f), 0));
  tree.insert(helper::OctTreeNodeData<int>(osg::Vec3f(-5.0f, -5.0f, 8.0f), 0));
  tree.insert(helper::OctTreeNodeData<int>(osg::Vec3f(-5.0f, 5.0f, 4.0f), 0));

  helper::OctTreeNode<int>::Data data;
  tree.gatherDataWithinBoundary(osg::BoundingBox(osg::Vec3f(-9.0f, -9.0f, -9.0f), osg::Vec3f(0.0f, 0.0f, 2.0f)), data);

  ASSERT_EQ(data.size(), 1);
}
