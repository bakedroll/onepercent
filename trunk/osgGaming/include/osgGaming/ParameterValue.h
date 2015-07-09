#pragma once

#include <osg/Referenced>

namespace osgGaming
{
	class AbstractParameterValue : public osg::Referenced
	{
	
	};

	template <typename T>
	class ParameterValue : public AbstractParameterValue
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