#include <osgGaming/FileResourceLoader.h>
#include <osgGaming/GameException.h>

using namespace osgGaming;
using namespace std;

void FileResourceLoader::getResourceStream(string resourceKey, ifstream& stream, long long& length)
{
	stream.open(resourceKey.c_str(), ios::binary);

	if (!stream.is_open())
	{
		throw GameException("Could not open file '" + resourceKey + "'");
	}

	stream.seekg(0, stream.end);
	length = stream.tellg();
	stream.seekg(0, stream.beg);
}