#pragma once

#include <osg/Referenced>
#include <osg/Vec2f>
#include <osg/Vec3i>

#include <string>

namespace onep
{
	class Country : public osg::Referenced
	{
	public:
		static const int SkillBranchCount = 5;

		typedef enum _skillBranchType
		{
			BRANCH_BANKS = 0,
			BRANCH_CONTROL = 1,
			BRANCH_MEDIA = 2,
			BRANCH_CORPORATIONS = 3,
			BRANCH_POLITICS = 4
		} SkillBranchType;

		Country(std::string name, unsigned char id, float population, int bip, osg::Vec2f center, osg::Vec2f size);

		void setSkillBranchActivated(SkillBranchType type, bool activated);

		std::string getCountryName();
		unsigned char getId();
		osg::Vec2f getCenterLatLong();
		osg::Vec2f getSize();
		int getBip();
		bool getSKillBranchActivated(SkillBranchType type);

	private:
		std::string _name;

		float _populationInMio;
		int _bip;
		float _anger;
		float _angerBalance;
		float _buyingPower;
		int _dept;

		bool _skillBranchActivated[SkillBranchCount];

		unsigned char _id;
		osg::Vec2f _centerLatLong;
		osg::Vec2f _size;
	};
}