#pragma once

#include <osg/Referenced>

namespace osgGaming
{
	class AbstractPropertyValue : public osg::Referenced
	{
	
	};

	template <typename T>
	class PropertyValue : public AbstractPropertyValue
	{
	public:
		T* getPtr()
		{
			return &_value;
		}

	private:
		T _value;
	};
}