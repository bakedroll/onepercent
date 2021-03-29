#pragma once

#include <osgGaming/ResourceLoader.h>

namespace osgGaming
{
	class FileResourceLoader : public ResourceLoader
	{
	public:
		void getResourceStream(std::string resourceKey, std::ifstream& stream, long long& length) override;
	};
}