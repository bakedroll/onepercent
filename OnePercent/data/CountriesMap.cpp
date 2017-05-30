#include "CountriesMap.h"

#include <string>
#include <cstring>

using namespace osg;
using namespace std;
using namespace onep;

CountriesMap::CountriesMap(int width, int height, unsigned char* data)
	: Referenced(),
	  _width(width),
	  _height(height)
{
	_data = new unsigned char[_width * _height];

	std::memcpy(&_data[0], data, _width * _height);
}

CountriesMap::~CountriesMap()
{
	delete[] _data;
}

Vec2i CountriesMap::getSize()
{
	return Vec2i(_width, _height);
}

unsigned char CountriesMap::getDataAt(int x, int y)
{
	return _data[y * _width + x];
}
