#pragma once

#include "nodes/CountryNode.h"

#include <osg/Referenced>
#include <osg/Array>
#include <osg/Geometry>

#include <osgGaming/Injector.h>

namespace onep
{

class BoundariesData : public osg::Referenced
{
public:
  explicit BoundariesData(osgGaming::Injector& injector);
  ~BoundariesData();

  void loadBoundaries(const std::string& filename);

  osg::ref_ptr<osg::Vec3Array> getCountryVertices() const;
  osg::ref_ptr<osg::Vec2Array> getCountryTexcoords() const;

  osg::ref_ptr<osg::Geometry> createOverallBoundariesGeometry(float thickness = 0.005f) const;
  osg::ref_ptr<osg::Geometry> createCountryBoundariesGeometry(const CountryNode::Map& countries,
                                                              const osg::Vec3f& color = osg::Vec3f(1.0f, 1.0f, 1.0f),
                                                              float thickness = 0.005f, bool bWireframe = false) const;

private:
  struct Impl;
  std::unique_ptr<Impl> m;

};

}  // namespace onep
