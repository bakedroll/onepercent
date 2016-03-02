#pragma once

#include <osg/Referenced>
#include <osg/Vec2f>
#include <osg/ref_ptr>

#include <string>

#include "ProgressingValue.h"
#include "SkillBranch.h"

namespace onep
{
	class NeighborCountryInfo : public osg::Referenced
	{
	public:
		float getRelation();
		void setRelation(float relation);

	private:
		float _relation;
	};

	class Country : public osg::Referenced
	{
	public:
		typedef osg::ref_ptr<Country> Ptr;

		static const int SkillBranchCount = 5;

		Country(std::string name, unsigned char id, float population, float wealth, osg::Vec2f center, osg::Vec2f size);

		void addAngerInfluence(float influence);
    void addInterestChange(float change);

		void setSkillBranchActivated(int type, bool activated);

		std::string getCountryName();
		unsigned char getId();
		osg::Vec2f getCenterLatLong();
		osg::Vec2f getSize();
		osg::Vec2f getSurfaceSize();
		float getOptimalCameraDistance(float angle, float ratio);

    ProgressingValue<float>::Ptr getAngerValue();
    ProgressingValue<float>::Ptr getDeptValue();
    ProgressingValue<float>::Ptr getInterestValue();
    ProgressingValue<float>::Ptr getBuyingPowerValue();

    ProgressingValue<float>::Ptr getAffectNeighborValue(SkillBranch::Type type);
    ProgressingValue<float>::Ptr getAffectedByNeighborValue(SkillBranch::Type type);

    bool getSkillBranchActivated(int type);

		bool anySkillBranchActivated();

		void step();

    void debugPrintValueString(std::string& str);
	private:
		std::string m_name;

    ProgressingValue<float>::Ptr m_valueAnger;
    ProgressingValue<float>::Ptr m_valueDept;
    ProgressingValue<float>::Ptr m_valueInterest;
    ProgressingValue<float>::Ptr m_valueBuyingPower;

    ProgressingValue<float>::Ptr m_valueAffectNeighbor[SkillBranchCount];
    ProgressingValue<float>::Ptr m_valueAffectedByNeighbor[SkillBranchCount];

    ProgressingValueContainer m_valueContainer;

		float m_populationInMio;

		bool m_skillBranchActivated[SkillBranchCount];

		unsigned char m_id;
		osg::Vec2f m_centerLatLong;
		osg::Vec2f m_size;
	};
}
