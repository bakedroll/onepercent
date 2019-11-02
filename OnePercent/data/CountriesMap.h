#pragma once

#include <osg/Vec2f>

#include <memory>
#include <vector>

namespace onep
{
	class CountriesMap
	{
	public:
    using Ptr = std::shared_ptr<CountriesMap>;

		CountriesMap();

    void initialize(int width, int height, unsigned char* data);
		unsigned char getDataAt(const osg::Vec2f& coord) const;

  private:
    int                        m_width;
    int                        m_height;
    std::vector<unsigned char> m_data;

  };
}
