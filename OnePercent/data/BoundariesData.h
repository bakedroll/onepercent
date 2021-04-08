#pragma once

#include <osg/Referenced>
#include <osg/Geometry>

#include <osgHelper/ioc/Injector.h>

namespace onep
{

class BoundariesData : public osg::Referenced
{
public:
  using BorderIdMap    = std::map<int, std::vector<int>>;
  using CountryBorders = std::map<int, BorderIdMap>;

  explicit BoundariesData(osgHelper::ioc::Injector& injector);
  ~BoundariesData();

  void loadBoundaries(const std::string& filename);

  osg::ref_ptr<osg::Geometry> createOverallBoundariesGeometry(float thickness = 0.005f) const;
  osg::ref_ptr<osg::Geometry> createCountryBoundariesGeometry(const CountryBorders& countryBorders,
                                                              const osg::Vec3f& color = osg::Vec3f(1.0f, 1.0f, 1.0f),
                                                              float thickness = 0.005f, bool bWireframe = false) const;

private:
  struct Impl;
  std::unique_ptr<Impl> m;

};

}  // namespace onep
