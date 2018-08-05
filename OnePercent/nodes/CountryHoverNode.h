#pragma once

#include <osg/Geode>

#include <memory>

namespace onep
{
  class CountryHoverNode : public osg::Geode
  {
  public:
    typedef osg::ref_ptr<CountryHoverNode> Ptr;
    typedef std::map<int, Ptr> Map;

    CountryHoverNode(osg::ref_ptr<osg::Vec3Array> vertices,
      osg::ref_ptr<osg::Vec2Array> texcoords,
      osg::ref_ptr<osg::DrawElementsUInt> triangles);

    ~CountryHoverNode();

    void setHoverEnabled(bool bEnabled);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}