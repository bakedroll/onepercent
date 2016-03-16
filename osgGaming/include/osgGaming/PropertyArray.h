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
    void addArray(int index, std::string name, osg::ref_ptr<PropertyArray> arr);

		bool hasProperty(int index, std::string name);
    bool hasArray(int index, std::string name);

		template <typename T>
		osg::ref_ptr<PropertyValue<T>> property(int index, std::string name)
		{
			return static_cast<PropertyValue<T>*>(m_propertyArray.find(index)->second.find(name)->second.get());
		}

    osg::ref_ptr<PropertyArray> array(int index, std::string name);

	private:
		typedef std::map<std::string, osg::ref_ptr<AbstractPropertyValue>> PropertyNameValueMap;
    typedef std::map<std::string, osg::ref_ptr<PropertyArray>> ArrayNameArrayMap;

		typedef std::map<int, PropertyNameValueMap> PropertyArrayMap;
    typedef std::map<int, ArrayNameArrayMap> ArrayArrayMap;

		PropertyArrayMap m_propertyArray;
    ArrayArrayMap m_arrayArray;
	};
}