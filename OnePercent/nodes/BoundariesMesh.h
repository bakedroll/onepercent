#pragma once

#include <osgGaming/Injector.h>

#include "nodes/CountryNode.h"

#include <osg/Group>

namespace onep
{
  class BoundariesData;

  class BoundariesMesh : public osg::Group
  {
  public:
    typedef osg::ref_ptr<BoundariesMesh> Ptr;

    BoundariesMesh(osgGaming::Injector& injector);
    ~BoundariesMesh();

    void makeOverallBoundaries(float thickness = 0.005f);
    void makeCountryBoundaries(
      const CountryNode::Map& countries,
      const osg::Vec3f& color = osg::Vec3f(1.0f, 1.0f, 1.0f),
      float thickness = 0.005f,
      bool bWireframe = false);

  private:
    osg::ref_ptr<osg::Geode>     m_overallBoundsGeode;
    osg::ref_ptr<osg::Geode>     m_countriesBoundsGeode;
    osg::ref_ptr<BoundariesData> m_boundariesData;
  };
}