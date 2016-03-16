#pragma once

#include <osg/Referenced>
#include <osg/Vec2f>
#include <osg/ref_ptr>
#include <osg/Group>

#include "CountryValues.h"
#include "SimulationCallback.h"

namespace onep
{
	class NeighborCountryInfo : public osg::Referenced
	{
	public:
    typedef osg::ref_ptr<NeighborCountryInfo> Ptr;

		float getRelation();
		void setRelation(float relation);

	private:
		float _relation;
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

		CountryData(std::string name, unsigned char id, float population, float wealth, osg::Vec2f center, osg::Vec2f size);

    void addNeighbor(Neighbor neighbor);

		void setSkillBranchActivated(int type, bool activated);

		std::string getCountryName();
		unsigned char getId();
		osg::Vec2f getCenterLatLong();
		osg::Vec2f getSize();
		osg::Vec2f getSurfaceSize();
		float getOptimalCameraDistance(float angle, float ratio);

    CountryValues::Ptr getValues();

    bool getSkillBranchActivated(int type);
    bool* getSkillBranchsActivated();

		bool anySkillBranchActivated();

    virtual bool callback(SimulationVisitor* visitor) override;

	private:
		std::string m_name;

    CountryValues::Ptr m_values;
    Neighbor::List m_neighbors;

		float m_populationInMio;

		bool m_skillBranchActivated[NUM_SKILLBRANCHES];

		unsigned char m_id;
		osg::Vec2f m_centerLatLong;
		osg::Vec2f m_size;

    void step();
    void affectNeighbors();
	};
}
