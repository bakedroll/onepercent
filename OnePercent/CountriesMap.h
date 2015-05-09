#pragma once

#include <osg/Referenced>
#include <osg/Vec3i>
#include <osg/Vec2i>

namespace onep
{
	class CountriesMap : public osg::Referenced
	{
	public:
		CountriesMap(int width, int height, unsigned char* data);
		~CountriesMap();

		osg::Vec2i getSize();

		osg::Vec3i getDataAt(int x, int y);

	private:
		int _width;
		int _height;

		unsigned char* _data;
	};
}