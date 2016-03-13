#pragma once

#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/Vec2i>

namespace onep
{
	class CountriesMap : public osg::Referenced
	{
	public:
    typedef osg::ref_ptr<CountriesMap> Ptr;

		CountriesMap(int width, int height, unsigned char* data);
		~CountriesMap();

		osg::Vec2i getSize();

		unsigned char getDataAt(int x, int y);

	private:
		int _width;
		int _height;

		unsigned char* _data;
	};
}