#pragma once

#include <string>
#include <map>

#include <osg/ref_ptr>

#include <osgGaming/PropertyNode.h>
#include <osgGaming/PropertyArray.h>
#include <osgGaming/PropertyValue.h>

namespace osgGaming
{
	class PropertyGroup : public PropertyNode
	{
	public:
		osg::ref_ptr<PropertyGroup> group(std::string name);
		osg::ref_ptr<PropertyArray> array(std::string name);

		template <typename T>
		osg::ref_ptr<PropertyValue<T>> property(std::string name)
		{
			return static_cast<PropertyValue<T>*>(_properties.find(name)->second.get());
		}

		void addGroup(std::string name, osg::ref_ptr<PropertyGroup> group);
		void addArray(std::string name, osg::ref_ptr<PropertyArray> a);
		void addProperty(std::string name, osg::ref_ptr<AbstractPropertyValue> value);

	private:
		typedef std::map<std::string, osg::ref_ptr<PropertyGroup>> GroupMap;
		typedef std::map<std::string, osg::ref_ptr<PropertyArray>> ArrayMap;
		typedef std::map<std::string, osg::ref_ptr<AbstractPropertyValue>> PropertyMap;

		GroupMap _groups;
		ArrayMap _arrays;
		PropertyMap _properties;
	};
}