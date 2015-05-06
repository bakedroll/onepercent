#pragma once

#include <string>

#include <osg/PositionAttitudeTransform>

namespace onep
{
	class BackgroundModel : public osg::PositionAttitudeTransform
	{
	public:
		BackgroundModel(std::string starsFilename);

	private:
		void makeStars(std::string starsFilename);
	};
}