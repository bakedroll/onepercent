#pragma once

#include <map>

#include <rapidxml.hpp>

#include <osgGaming/Singleton.h>
#include <osgGaming/PropertyValue.h>
#include <osgGaming/PropertyGroup.h>

namespace osgGaming
{
	class PropertiesManager : public Singleton<PropertiesManager>
	{
	public:
		PropertiesManager();

		template <typename T>
		T* getValuePtr(std::string name)
		{
			AbstractPropertyValue::ValueMap::iterator it = _values.find(name);
			if (it == _values.end())
			{
				PropertyValue<T>* value = new PropertyValue<T>();
				_values.insert(AbstractPropertyValue::ValueMap::value_type(name, value));

				return value->getPtr();
			}

			return static_cast<PropertyValue<T>*>(it->second.get())->getPtr();
		}

		osg::ref_ptr<PropertyGroup> root();

		void loadPropertiesFromXmlResource(std::string resourceKey);

	private:
		typedef struct _arrayField
		{
			std::string type;
			std::string defaultValue;
		} ArrayField;

		typedef std::map<std::string, ArrayField> ArrayFieldMap;

		typedef struct _arrayContext
		{
			int index;
			osg::ref_ptr<PropertyArray> propertyArray;
			ArrayFieldMap fields;
		} ArrayContext;

		void initializeProperty(std::string path, std::string type, std::string value);;
		void parseXmlNode(rapidxml::xml_node<>* node, osg::ref_ptr<PropertyGroup> group, std::string path, ArrayContext* arrayContext = nullptr);
		void parseXmlGroup(rapidxml::xml_node<>* node, osg::ref_ptr<PropertyGroup> group, std::string path);
    void parseXmlArray(rapidxml::xml_node<>* node, osg::ref_ptr<PropertyNode> pnode, std::string path, ArrayContext* arrayContext = nullptr);
		void parseXmlProperty(rapidxml::xml_node<>* node, osg::ref_ptr<PropertyGroup> group, std::string path, ArrayContext* arrayContext = nullptr);
		void parseXmlArrayFields(rapidxml::xml_node<>* node, const std::string& path, ArrayContext* arrayContext);
		void parseXmlArrayElements(rapidxml::xml_node<>* node, std::string path, ArrayContext* context);

		void throwMissingAttribute(const std::string& path, const std::string& tag);
		void throwMissingAttribute(const std::string& path, const std::string& tag, const std::string& attribute);

		AbstractPropertyValue::ValueMap _values;

		osg::ref_ptr<PropertyGroup> _root;
	};
}