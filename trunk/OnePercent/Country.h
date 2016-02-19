#pragma once

#include <osg/Referenced>
#include <osg/Vec2f>
#include <osg/ref_ptr>

#include <string>

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

		typedef enum _skillBranchType
		{
			BRANCH_BANKS = 0,
			BRANCH_CONTROL = 1,
			BRANCH_MEDIA = 2,
			BRANCH_CONCERNS = 3,
			BRANCH_POLITICS = 4
		} SkillBranchType;

		Country(std::string name, unsigned char id, float population, float wealth, osg::Vec2f center, osg::Vec2f size);

		void addAngerInfluence(float influence);
		void addInterestInfluence(float influence);

		void setSkillBranchActivated(SkillBranchType type, bool activated);

		std::string getCountryName();
		unsigned char getId();
		osg::Vec2f getCenterLatLong();
		osg::Vec2f getSize();
		osg::Vec2f getSurfaceSize();
		float getOptimalCameraDistance(float angle, float ratio);

		float getWealth();
		float getDept();
		float getDeptBalance();
		float getRelativeDept();
		float getAnger();
		float getAngerBalance();

		bool getSKillBranchActivated(SkillBranchType type);

		bool anySkillBranchActivated();

		void clearEffects();

		void step();
	private:
		std::string _name;

		float _populationInMio;
		float _wealth;
		float _anger;
		float _angerBalance;
		float _buyingPower;
		float _dept;
		float _deptBalance;
		float _interest;

		float _angerInfluence;
		float _interestInfluence;

		bool _skillBranchActivated[SkillBranchCount];

		unsigned char _id;
		osg::Vec2f _centerLatLong;
		osg::Vec2f _size;
	};
}