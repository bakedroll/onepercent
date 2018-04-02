#pragma once

#include "SkillsContainer.h"

#include <osg/Referenced>
#include <osg/Vec2f>
#include <osg/ref_ptr>
#include <osg/Group>

#include <osgGaming/PropertiesManager.h>

namespace onep
{
  class SkillsContainer;

	class NeighborCountryInfo : public osg::Referenced
	{
	public:
    typedef osg::ref_ptr<NeighborCountryInfo> Ptr;

		float getRelation();
		void setRelation(float relation);

	private:
		float m_relation;
	};

	class CountryData : public osg::Referenced
	{
	public:
		typedef osg::ref_ptr<CountryData> Ptr;

    typedef struct _neighbor
    {
      typedef std::vector<_neighbor> List;

      osg::ref_ptr<CountryData> country;
      osg::ref_ptr<NeighborCountryInfo> info;
    } Neighbor;

		CountryData(
      osg::ref_ptr<osgGaming::PropertiesManager> propertiesManager,
      osg::ref_ptr<SkillsContainer> skillsContainer,
      std::string name,
      int id,
      float population,
      float wealth,
      osg::Vec2f center,
      osg::Vec2f size);

    ~CountryData();

    void addNeighbor(Neighbor neighbor);

		std::string getCountryName();
    int getId();
		osg::Vec2f getCenterLatLong();
		osg::Vec2f getSize();
		osg::Vec2f getSurfaceSize();
		float getOptimalCameraDistance(float angle, float ratio);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
