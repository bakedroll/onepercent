#include <osgGaming/FileResourceLoader.h>
#include <osgGaming/GameException.h>

namespace osgGaming
{

void FileResourceLoader::getResourceStream(std::string resourceKey, std::ifstream& stream, long long& length)
{
  stream.open(resourceKey.c_str(), std::ios::binary);

  if (!stream.is_open())
  {
    throw GameException("Could not open file '" + resourceKey + "'");
  }

  stream.seekg(0, stream.end);
  length = stream.tellg();
  stream.seekg(0, stream.beg);
}

}  // namespace osgHelper