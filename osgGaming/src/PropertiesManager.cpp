#include <osgGaming/PropertiesManager.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/GameException.h>
#include <osgGaming/Helper.h>

using namespace osgGaming;
using namespace osg;
using namespace std;
using namespace rapidxml;

ref_ptr<PropertiesManager> Singleton<PropertiesManager>::_instance;

void PropertiesManager::loadPropertiesFromXmlResource(string resourceKey)
{
	string xmlText = ResourceManager::getInstance()->loadText(resourceKey);

	//locale loc = locale::global(locale(locale("de"), new codecvt_utf8<char>));

	char* xmlCstr = new char[xmlText.size() + 1];
	strcpy(xmlCstr, xmlText.c_str());

	//locale loc = locale::global(locale(locale(), new codecvt_utf8<char>));

	xml_document<> doc;
	doc.parse<0>(xmlCstr);

	xml_node<>* rootNode = doc.first_node("root");

	if (rootNode == nullptr)
	{
		throw GameException("Invalid resource format: " + resourceKey);
	}

	parseXmlGroup(rootNode, "");

	//locale::global(loc);

	delete[] xmlCstr;
	ResourceManager::getInstance()->clearCacheResource(resourceKey);
}

void PropertiesManager::parseXmlGroup(xml_node<>* node, std::string path, ArrayContext* arrayContext)
{
	if (arrayContext == nullptr)
	{
		xml_node<>* groupChild = node->first_node("group");
		while (groupChild != nullptr)
		{
			xml_attribute<>* attr_name = groupChild->first_attribute("name");

			if (attr_name == nullptr)
			{
				throwMissingAttribute(path, "group", "name");
			}

			parseXmlGroup(groupChild, path + attr_name->value() + "/");

			groupChild = groupChild->next_sibling("group");
		}

		xml_node<>* arrayChild = node->first_node("array");
		while (arrayChild != nullptr)
		{
			xml_attribute<>* attr_name = arrayChild->first_attribute("name");

			if (attr_name == nullptr)
			{
				throwMissingAttribute(path, "array", "name");
			}

			ArrayContext context;
			std::string p = path + attr_name->value();
			parseXmlArrayFields(arrayChild, p, &context);
			parseXmlArrayElements(arrayChild, p, &context);

			arrayChild = arrayChild->next_sibling("array");
		}
	}

	xml_node<>* propertyChild = node->first_node("property");
	while (propertyChild != nullptr)
	{
		xml_attribute<>* attr_name = propertyChild->first_attribute("name");
		//xml_attribute<>* attr_type = propertyChild->first_attribute("type");
		xml_attribute<>* attr_value = propertyChild->first_attribute("value");

		if (attr_name == nullptr || attr_value == nullptr)
		{
			throwMissingAttribute(path, "property");
		}

		std::string name = attr_name->value();
		std::string type;

		if (arrayContext != nullptr)
		{
			ArrayContext::iterator it = arrayContext->find(name);
			if (it == arrayContext->end())
			{
				propertyChild = propertyChild->next_sibling("property");
				continue;
			}

			type = it->second;
		}
		else
		{
			xml_attribute<>* attr_type = propertyChild->first_attribute("type");

			if (attr_type == nullptr)
			{
				throwMissingAttribute(path, "property", "type");
			}

			type = string(attr_type->value());
		}

		if (strcmp(type.c_str(), "string") == 0)
		{
			*getValuePtr<string>(path + name) = utf8ToLatin1(attr_value->value());
		}
		else if (strcmp(type.c_str(), "int") == 0)
		{
			*getValuePtr<int>(path + name) = atoi(attr_value->value());
		}
		else if (strcmp(type.c_str(), "float") == 0)
		{
			*getValuePtr<float>(path + name) = float(atof(attr_value->value()));
		}
		else if (strcmp(type.c_str(), "double") == 0)
		{
			*getValuePtr<double>(path + name) = atof(attr_value->value());
		}
		else if (strcmp(type.c_str(), "vec2") == 0)
		{
			string val = string(attr_value->value());

			*getValuePtr<Vec2f>(path + name) = parseVector<Vec2f>(val);
		}
		else if (strcmp(type.c_str(), "vec3") == 0)
		{
			string val = string(attr_value->value());

			*getValuePtr<Vec3f>(path + name) = parseVector<Vec3f>(val);
		}
		else if (strcmp(type.c_str(), "vec4") == 0)
		{
			string val = string(attr_value->value());

			*getValuePtr<Vec4f>(path + name) = parseVector<Vec4f>(val);
		}

		propertyChild = propertyChild->next_sibling("property");
	}
}

void PropertiesManager::parseXmlArrayFields(xml_node<>* node, const std::string& path, ArrayContext* arrayContext)
{
	xml_node<>* fieldChild = node->first_node("field");
	while (fieldChild != nullptr)
	{
		xml_attribute<>* attr_name = fieldChild->first_attribute("name");
		xml_attribute<>* attr_type = fieldChild->first_attribute("type");
		
		if (attr_name == nullptr || attr_type == nullptr)
		{
			throwMissingAttribute(path, "field");
		}

		string name = string(attr_name->value());
		string type = string(attr_type->value());

		if (arrayContext->find(name) != arrayContext->end())
		{
			throw GameException("Field " + name + " already exists at " + path);
		}

		arrayContext->insert(ArrayContext::value_type(name, type));

		fieldChild = fieldChild->next_sibling("field");
	}
}

void PropertiesManager::parseXmlArrayElements(rapidxml::xml_node<>* node, std::string path, ArrayContext* context)
{
	int counter = 0;

	xml_node<>* elementChild = node->first_node("element");
	while (elementChild != nullptr)
	{
		char ac[16];
		sprintf(ac, "[%d]", counter);

		parseXmlGroup(elementChild, path + string(ac) + "/", context);

		counter++;
		elementChild = elementChild->next_sibling("element");
	}
}

void PropertiesManager::throwMissingAttribute(const std::string& path, const std::string& tag)
{
	throw GameException("Missing attribute(s) in " + tag + " tag at " + path);
}

void PropertiesManager::throwMissingAttribute(const std::string& path, const std::string& tag, const std::string& attribute)
{
	throw GameException("Missing attribute " + attribute + " in " + tag + " tag at " + path);
}