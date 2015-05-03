#include <osgGaming/PackageResourceLoader.h>
#include <osgGaming/GameException.h>
#include <osgGaming/Helper.h>

#include <fstream>
#include <string>

using namespace osgGaming;
using namespace std;

void PackageResourceLoader::registerPackage(string filename)
{
	ifstream is;
	is.open(filename, ios::binary);
	if (!is.is_open())
	{
		throw GameException("Package file '" + filename + "' cannot be opened");
	}

	int resourcesCount;

	is.read((char*)&resourcesCount, sizeof(int));

	for (int i = 0; i < resourcesCount; i++)
	{
		PakFile pakFile;

		int keyLength;
		is.read((char*)&keyLength, sizeof(int));

		char* resourceKeyBuffer = new char[keyLength + 1];

		is.read(resourceKeyBuffer, keyLength);
		resourceKeyBuffer[keyLength] = '\0';

		is.read((char*)&pakFile.pos, sizeof(long long));
		is.read((char*)&pakFile.size, sizeof(long long));

		pakFile.packageFilename = filename;

		string key = lowerString(string(resourceKeyBuffer));

		if (_pakFiles.find(key) != _pakFiles.end())
		{
			throw GameException("Resource key '" + key + "' already registered");
		}

		_pakFiles.insert(ResourceKeyPakFileDictionary::value_type(key, pakFile));

		delete[] resourceKeyBuffer;
	}

	is.close();
}

void PackageResourceLoader::getResourceStream(string resourceKey, ifstream& stream, long long& length)
{
	string key = lowerString(resourceKey);

	ResourceKeyPakFileDictionary::iterator it = _pakFiles.find(key);
	if (it == _pakFiles.end())
	{
		throw GameException("Resource key '" + resourceKey + "' not found");
	}

	stream.open(it->second.packageFilename, ios::binary);
	if (!stream.is_open())
	{
		throw GameException("Package file '" + it->second.packageFilename + "' cannot be opened");
	}

	stream.seekg(it->second.pos);
	length = it->second.size;
}
