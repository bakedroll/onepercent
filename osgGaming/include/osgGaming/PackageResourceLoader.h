#pragma once

#include <map>
#include <string>

#include <osgGaming/ResourceLoader.h>

namespace osgGaming
{
	class PackageResourceLoader : public ResourceLoader
	{
	public:
		void registerPackage(std::string filename);

		void getResourceStream(std::string resourceKey, std::ifstream& stream, long long& length) override;

	private:
		typedef struct _pakFile
		{
			std::string packageFilename;
			long long pos;
			long long size;
		} PakFile;

		typedef std::map<std::string, PakFile> ResourceKeyPakFileDictionary;

		ResourceKeyPakFileDictionary _pakFiles;
	};
}