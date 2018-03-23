#pragma once

#include "SkillBranchContainer.h"

#include <osg/Referenced>
#include <osg/Vec2f>
#include <osg/ref_ptr>
#include <osg/Group>

#include <osgGaming/Observable.h>
#include <osgGaming/PropertiesManager.h>

#include "CountryValues.h"
#include "SimulationCallback.h"

namespace onep
{
  class SkillBranchContainer;

	class NeighborCountryInfo : public osg::Referenced
	{
	public:
    typedef osg::ref_ptr<NeighborCountryInfo> Ptr;

		float getRelation();
		void setRelation(float relation);

	private:
		float m_relation;
	};

	class CountryData : public osg::Group, public SimulationCallback
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
      osg::ref_ptr<SkillBranchContainer> skillBranchContainer,
      std::string name,
      int id,
      float population,
      float wealth,
      osg::Vec2f center,
      osg::Vec2f size);

    ~CountryData();

    void addNeighbor(Neighbor neighbor);

		void setSkillBranchActivated(int type, bool activated);

		std::string getCountryName();
    int getId();
		osg::Vec2f getCenterLatLong();
		osg::Vec2f getSize();
		osg::Vec2f getSurfaceSize();
		float getOptimalCameraDistance(float angle, float ratio);

    CountryValues::Ptr getValues();

    bool getSkillBranchActivated(int id);
    osgGaming::Observable<bool>::Ptr getSkillBranchActivatedObservable(int type);

		bool anySkillBranchActivated();

    virtual bool callback(SimulationVisitor* visitor) override;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
