#include "CountriesMap.h"

namespace onep
{
  CountriesMap::CountriesMap(int width, int height, unsigned char* data)
	  : m_width(width)
	  , m_height(height)
  {
    m_data.resize(m_width * m_height);
	  std::memcpy(m_data.data(), data, m_width * m_height);
  }

  osg::Vec2i CountriesMap::getSize() const
  {
	  return osg::Vec2i(m_width, m_height);
  }

  unsigned char CountriesMap::getDataAt(int x, int y) const
  {
	  return m_data[y * m_width + x];
  }
}