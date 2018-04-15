#pragma once

#include <osgGaming/Injector.h>

#include "nodes/CountryMesh.h"

#include <osg/Group>
#include <memory>

namespace onep
{
  class BoundariesMesh : public osg::Group
  {
  public:
    typedef osg::ref_ptr<BoundariesMesh> Ptr;

    BoundariesMesh(osgGaming::Injector& injector);
    ~BoundariesMesh();

    void loadBoundaries(const std::string& filename);

    osg::ref_ptr<osg::Vec3Array> getCountryVertices();
    osg::ref_ptr<osg::Vec2Array> getCountryTexcoords();

    void makeOverallBoundaries(float thickness = 0.005f);
    void makeCountryBoundaries(
      const CountryMesh::Map& countries,
      const osg::Vec3f& color = osg::Vec3f(1.0f, 1.0f, 1.0f),
      float thickness = 0.005f);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}