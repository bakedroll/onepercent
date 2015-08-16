#pragma once

#include <string>
#include <map>
#include <vector>

#include <rapidxml.hpp>

#include <osgGaming/Singleton.h>
#include <osgGaming/ParameterValue.h>

namespace osgGaming
{
	class ParameterManager : public Singleton<ParameterManager>
	{
	public:
		template <typename T>
		T* getValuePtr(std::string name)
		{
			ValueMap::iterator it = _values.find(name);
			if (it == _values.end())
			{
				ParameterValue<T>* value = new ParameterValue<T>();
				_values.insert(ValueMap::value_type(name, value));

				return value->getPtr();
			}

			return static_cast<ParameterValue<T>*>(it->second.get())->getPtr();
		}

		void loadParametersFromXmlResource(std::string resourceKey);

	private:
		typedef std::map<std::string, std::string> ArrayContext;
		typedef std::map<std::string, osg::ref_ptr<AbstractParameterValue>> ValueMap;

		void parseXmlGroup(rapidxml::xml_node<>* node, std::string path, ArrayContext* arrayContext = nullptr);
		void parseXmlArrayFields(rapidxml::xml_node<>* node, const std::string& path, ArrayContext* arrayContext);
		void parseXmlArrayElements(rapidxml::xml_node<>* node, std::string path, ArrayContext* context);

		void throwMissingAttribute(const std::string& path, const std::string& tag);
		void throwMissingAttribute(const std::string& path, const std::string& tag, const std::string& attribute);

		ValueMap _values;
	};
}