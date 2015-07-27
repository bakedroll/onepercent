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

	//locale loc = locale::global(locale(locale("de"), new codecvt_utf8<char>));

	char* xmlCstr = new char[xmlText.size() + 1];
	strcpy(xmlCstr, xmlText.c_str());

	//locale loc = locale::global(locale(locale(), new codecvt_utf8<char>));

	xml_document<> doc;
	doc.parse<0>(xmlCstr);

	xml_node<>* rootNode = doc.first_node("root");

	if (rootNode == NULL)
	{
		throw GameException("Invalid resource format: " + resourceKey);
	}

	parseXmlGroup(rootNode, "");

	//locale::global(loc);

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

		groupChild = groupChild->next_sibling("group");
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
			*getValuePtr<string>(path + attr_name->value()) = utf8ToLatin1(attr_value->value());
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
			*getValuePtr<Vec2f>(path + attr_name->value()) = parseVector<Vec2f>(string(attr_value->value()));
		}
		else if (strcmp(attr_type->value(), "vec3") == 0)
		{
			*getValuePtr<Vec3f>(path + attr_name->value()) = parseVector<Vec3f>(string(attr_value->value()));
		}
		else if (strcmp(attr_type->value(), "vec4") == 0)
		{
			*getValuePtr<Vec4f>(path + attr_name->value()) = parseVector<Vec4f>(string(attr_value->value()));
		}

		parameterChild = parameterChild->next_sibling("parameter");
	}
}