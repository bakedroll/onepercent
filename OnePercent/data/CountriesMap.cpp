#include "CountriesMap.h"

#include <cstring>

namespace onep
{
  CountriesMap::CountriesMap()
	  : m_width(0)
	  , m_height(0)
  {
    
  }

  void CountriesMap::initialize(int width, int height, unsigned char* data)
  {
    m_width  = width;
    m_height = height;

    m_data.resize(m_width * m_height);
	  std::memcpy(m_data.data(), data, m_width * m_height);
  }

  unsigned char CountriesMap::getDataAt(const osg::Vec2f& coord) const
  {
    auto ix = static_cast<int>(coord.x() * m_width);
    auto iy = static_cast<int>(coord.y() * m_height);

	  return m_data[iy * m_width + ix];
  }
}