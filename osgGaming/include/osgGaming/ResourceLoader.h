#pragma once

#include <string>
#include <fstream>

#include <osg/Referenced>

namespace osgGaming
{
	class ResourceLoader : public osg::Referenced
	{
	public:
		virtual void getResourceStream(std::string resourceKey, std::ifstream& stream, long long& length) = 0;
	};
}