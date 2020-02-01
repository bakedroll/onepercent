#include "nodes/BoundariesMesh.h"

#include "data/BoundariesData.h"

namespace onep
{
BoundariesMesh::BoundariesMesh(osgGaming::Injector& injector)
  : osg::Group()
  , m_boundariesData(injector.inject<BoundariesData>())
{
}

BoundariesMesh::~BoundariesMesh() = default;

void BoundariesMesh::makeOverallBoundaries(float thickness)
{
  if (m_overallBoundsGeode.valid())
  {
    return;
  }

  m_overallBoundsGeode = new osg::Geode();
  m_overallBoundsGeode->addDrawable(m_boundariesData->createOverallBoundariesGeometry(thickness));
  addChild(m_overallBoundsGeode);
}

void BoundariesMesh::makeCountryBoundaries(const CountryNode::Map& countries, const osg::Vec3f& color, float thickness,
                                           bool bWireframe)
{
  if (m_countriesBoundsGeode.valid())
  {
    removeChild(m_countriesBoundsGeode);
    m_countriesBoundsGeode.release();
  }

  m_countriesBoundsGeode = new osg::Geode();
  m_countriesBoundsGeode->addDrawable(m_boundariesData->createCountryBoundariesGeometry(countries, color, thickness, bWireframe));

  addChild(m_countriesBoundsGeode);
}
}  // namespace onep