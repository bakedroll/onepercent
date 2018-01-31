#pragma once

#include <map>
#include <memory>

#include <rapidxml.hpp>

#include <osgGaming/PropertyValue.h>
#include <osgGaming/PropertyGroup.h>

namespace osgGaming
{
  class Injector;

  class PropertiesManager : public osg::Referenced
	{
	public:
    explicit PropertiesManager(Injector& injector);
    ~PropertiesManager();

		template <typename T>
		T* getValuePtr(const std::string& name)
		{
			AbstractPropertyValue::ValueMap::iterator it = m_values.find(name);
			if (it == m_values.end())
			{
				PropertyValue<T>* value = new PropertyValue<T>();
				m_values.insert(AbstractPropertyValue::ValueMap::value_type(name, value));

				return value->getPtr();
			}

			return static_cast<PropertyValue<T>*>(it->second.get())->getPtr();
		}

		template <typename T>
		T getValue(const std::string& name)
		{
		  return *getValuePtr<T>(name);
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

		AbstractPropertyValue::ValueMap m_values;

    struct Impl;
    std::unique_ptr<Impl> m;
	};
}
