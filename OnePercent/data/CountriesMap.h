#pragma once

#include <osg/Vec2i>

#include <memory>
#include <vector>

namespace onep
{
	class CountriesMap
	{
	public:
    using Ptr = std::shared_ptr<CountriesMap>;

		CountriesMap(int width, int height, unsigned char* data);

		osg::Vec2i getSize() const;

		unsigned char getDataAt(int x, int y) const;

  private:
    int                        m_width;
    int                        m_height;
    std::vector<unsigned char> m_data;

  };
}
