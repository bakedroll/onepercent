#pragma once

#include <osgGaming/PropertyNode.h>

#include <osg/ref_ptr>

namespace osgGaming
{
	class AbstractPropertyValue : public PropertyNode
	{
	public:
		typedef std::map<std::string, osg::ref_ptr<AbstractPropertyValue>> ValueMap;
	};

	template <typename T>
	class PropertyValue : public AbstractPropertyValue
	{
	public:
		T get()
		{
			return _value;
		}

		T* getPtr()
		{
			return &_value;
		}

	private:
		T _value;
	};
}