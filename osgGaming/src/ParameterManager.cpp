#include <osgGaming/ParameterManager.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/GameException.h>
#include <osgGaming/Helper.h>

using namespace osgGaming;
using namespace osg;
using namespace std;
using namespace rapidxml;

ref_ptr<ParameterManager> Singleton<ParameterManager>::_instance;

void ParameterManager::loadParametersFromXmlResource(string resourceKey)
{
	string xmlText = ResourceManager::getInstance()->loadText(resourceKey);
	char* xmlCstr = new char[xmlText.size() + 1];
	strcpy(xmlCstr, xmlText.c_str());

	xml_document<> doc;
	doc.parse<0>(xmlCstr);

	xml_node<>* rootNode = doc.first_node("root");

	if (rootNode == NULL)
	{
		throw GameException("Invalid resource format: " + resourceKey);
	}

	parseXmlGroup(rootNode, "");

	delete[] xmlCstr;
	ResourceManager::getInstance()->clearCacheResource(resourceKey);
}

void ParameterManager::parseXmlGroup(xml_node<>* node, std::string path)
{
	xml_node<>* groupChild = node->first_node("group");
	while (groupChild != NULL)
	{
		xml_attribute<>* attr_name = groupChild->first_attribute("name");

		parseXmlGroup(groupChild, path + attr_name->value() + "/");

		groupChild = groupChild->next_sibling();
	}

	xml_node<>* parameterChild = node->first_node("parameter");
	while (parameterChild != NULL)
	{
		xml_attribute<>* attr_name = parameterChild->first_attribute("name");
		xml_attribute<>* attr_type = parameterChild->first_attribute("type");
		xml_attribute<>* attr_value = parameterChild->first_attribute("value");

		if (attr_name == NULL || attr_type == NULL || attr_value == NULL)
		{
			throw GameException("Missing attribute(s) in parameter tag");
		}

		if (strcmp(attr_type->value(), "string") == 0)
		{
			*getValuePtr<string>(path + attr_name->value()) = attr_value->value();
		}
		else if (strcmp(attr_type->value(), "int") == 0)
		{
			*getValuePtr<int>(path + attr_name->value()) = atoi(attr_value->value());
		}
		else if (strcmp(attr_type->value(), "float") == 0)
		{
			*getValuePtr<float>(path + attr_name->value()) = (float)atof(attr_value->value());
		}
		else if (strcmp(attr_type->value(), "double") == 0)
		{
			*getValuePtr<double>(path + attr_name->value()) = atof(attr_value->value());
		}
		else if (strcmp(attr_type->value(), "vec2") == 0)
		{
			StringList values = splitString(string(attr_value->value()), ',');

			if (values.size() < 2)
			{
				throw GameException("Not enough values in 'value' attribute");
			}

			*getValuePtr<Vec2f>(path + attr_name->value()) = Vec2f(
				stof(values[0]),
				stof(values[1]));
		}
		else if (strcmp(attr_type->value(), "vec3") == 0)
		{
			StringList values = splitString(string(attr_value->value()), ',');

			if (values.size() < 3)
			{
				throw GameException("Not enough values in 'value' attribute");
			}

			*getValuePtr<Vec3f>(path + attr_name->value()) = Vec3f(
				stof(values[0]),
				stof(values[1]),
				stof(values[2]));
		}
		else if (strcmp(attr_type->value(), "vec4") == 0)
		{
			StringList values = splitString(string(attr_value->value()), ',');

			if (values.size() < 4)
			{
				throw GameException("Not enough values in 'value' attribute");
			}

			*getValuePtr<Vec4f>(path + attr_name->value()) = Vec4f(
				stof(values[0]),
				stof(values[1]),
				stof(values[2]),
				stof(values[3]));
		}

		parameterChild = parameterChild->next_sibling();
	}
}