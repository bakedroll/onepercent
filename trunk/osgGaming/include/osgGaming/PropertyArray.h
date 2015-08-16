#pragma once

#include <osg/ref_ptr>
#include <string>
#include <map>

#include <osgGaming/PropertyNode.h>
#include <osgGaming/PropertyValue.h>

namespace osgGaming
{
	class PropertyArray : public PropertyNode
	{
	public:
		int size();
		void addProperty(int index, std::string name, osg::ref_ptr<AbstractPropertyValue> value);

		bool hasProperty(int index, std::string name);

		template <typename T>
		osg::ref_ptr<PropertyValue<T>> property(int index, std::string name)
		{
			return static_cast<PropertyValue<T>*>(_array.find(index)->second.find(name)->second.get());
		}

	private:
		typedef std::map<std::string, osg::ref_ptr<AbstractPropertyValue>> PropertyNameValueMap;
		typedef std::map<int, PropertyNameValueMap> PropertyArrayMap;

		PropertyArrayMap _array;
	};
}