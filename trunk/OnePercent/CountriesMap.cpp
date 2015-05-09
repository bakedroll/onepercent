#include "CountriesMap.h"

#include <string>

using namespace osg;
using namespace std;
using namespace onep;

CountriesMap::CountriesMap(int width, int height, unsigned char* data)
	: Referenced(),
	  _width(width),
	  _height(height)
{
	_data = new unsigned char[_width * _height * 3];

	memcpy(&_data[0], data, _width * _height * 3);
}

CountriesMap::~CountriesMap()
{
	delete[] _data;
}

Vec2i CountriesMap::getSize()
{
	return Vec2i(_width, _height);
}

osg::Vec3i CountriesMap::getDataAt(int x, int y)
{
	int pos = (y * _width * 3) + (x * 3);

	return Vec3i((int)_data[pos], (int)_data[pos + 1], (int)_data[pos + 2]);
}