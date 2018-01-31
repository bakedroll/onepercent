#include <osgGaming/Injector.h>
#include <osgGaming/PropertiesManager.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/GameException.h>
#include <osgGaming/Helper.h>

namespace osgGaming
{
  struct PropertiesManager::Impl
  {
    Impl(Injector& injector)
      : root(new PropertyGroup())
      , resourceManager(injector.inject<ResourceManager>())
    {}

    osg::ref_ptr<PropertyGroup> root;
    osg::ref_ptr<ResourceManager> resourceManager;
  };

  PropertiesManager::PropertiesManager(Injector& injector)
    : m(new Impl(injector))
  {
  }

  PropertiesManager::~PropertiesManager()
  {
  }

  osg::ref_ptr<PropertyGroup> PropertiesManager::root()
  {
    return m->root;
  }

  void PropertiesManager::loadPropertiesFromXmlResource(std::string resourceKey)
  {
    std::string xmlText = m->resourceManager->loadText(resourceKey);

    char* xmlCstr = new char[xmlText.size() + 1];
    strcpy(xmlCstr, xmlText.c_str());

    rapidxml::xml_document<> doc;
    doc.parse<0>(xmlCstr);

    rapidxml::xml_node<>* rootNode = doc.first_node("root");

    if (rootNode == nullptr)
    {
      throw GameException("Invalid resource format: " + resourceKey);
    }

    parseXmlNode(rootNode, m->root, "");

    delete[] xmlCstr;
    m->resourceManager->clearCacheResource(resourceKey);
  }

  void PropertiesManager::initializeProperty(std::string path, std::string type, std::string value)
  {
    if (type == "string")
    {
      *getValuePtr<std::string>(path) = utf8ToLatin1(value.c_str());
    }
    else if (type == "int")
    {
      *getValuePtr<int>(path) = stoi(value);
    }
    else if (type == "float")
    {
      *getValuePtr<float>(path) = stof(value);
    }
    else if (type == "double")
    {
      *getValuePtr<double>(path) = stod(value);
    }
    else if (type == "vec2")
    {
      *getValuePtr<osg::Vec2f>(path) = parseVector<osg::Vec2f>(value);
    }
    else if (type == "vec3")
    {
      *getValuePtr<osg::Vec3f>(path) = parseVector<osg::Vec3f>(value);
    }
    else if (type == "vec4")
    {
      *getValuePtr<osg::Vec4f>(path) = parseVector<osg::Vec4f>(value);
    }
    else if (type == "bool")
    {
      *getValuePtr<bool>(path) = value == "true" ? true : false;
    }
  }

  void PropertiesManager::parseXmlNode(rapidxml::xml_node<>* node, osg::ref_ptr<PropertyGroup> group, std::string path, ArrayContext* arrayContext)
  {
    if (arrayContext == nullptr)
    {
      parseXmlGroup(node, group, path);
      parseXmlArray(node, group, path);
    }
    else
    {
      parseXmlArray(node, arrayContext->propertyArray, path, arrayContext);
    }

    parseXmlProperty(node, group, path, arrayContext);
  }

  void PropertiesManager::parseXmlGroup(rapidxml::xml_node<>* node, osg::ref_ptr<PropertyGroup> group, std::string path)
  {
    rapidxml::xml_node<>* groupChild = node->first_node("group");
    while (groupChild != nullptr)
    {
      rapidxml::xml_attribute<>* attr_name = groupChild->first_attribute("name");

      if (attr_name == nullptr)
      {
        throwMissingAttribute(path, "group", "name");
      }

      std::string name = std::string(attr_name->value());

      group->addGroup(name, new PropertyGroup());
      parseXmlNode(groupChild, group->group(name), path + name + "/");

      groupChild = groupChild->next_sibling("group");
    }
  }

  void PropertiesManager::parseXmlArray(rapidxml::xml_node<>* node, osg::ref_ptr<PropertyNode> pnode, std::string path, ArrayContext* arrayContext)
  {
    rapidxml::xml_node<>* arrayChild = node->first_node("array");
    while (arrayChild != nullptr)
    {
      rapidxml::xml_attribute<>* attr_name = arrayChild->first_attribute("name");

      if (attr_name == nullptr)
      {
        throwMissingAttribute(path, "array", "name");
      }

      std::string name = attr_name->value();
      ArrayContext context;
      std::string p = path + name;

      context.propertyArray = new PropertyArray();

      PropertyGroup* group = dynamic_cast<PropertyGroup*>(pnode.get());
      if (group)
        group->addArray(name, context.propertyArray);

      PropertyArray* arr = dynamic_cast<PropertyArray*>(pnode.get());
      if (arr && arrayContext)
        arr->addArray(arrayContext->index, name, context.propertyArray);

      parseXmlArrayFields(arrayChild, p, &context);
      parseXmlArrayElements(arrayChild, p, &context);

      arrayChild = arrayChild->next_sibling("array");
    }
  }

  void PropertiesManager::parseXmlProperty(rapidxml::xml_node<>* node, osg::ref_ptr<PropertyGroup> group, std::string path, ArrayContext* arrayContext)
  {
    rapidxml::xml_node<>* propertyChild = node->first_node("property");
    while (propertyChild != nullptr)
    {
      rapidxml::xml_attribute<>* attr_name = propertyChild->first_attribute("name");
      rapidxml::xml_attribute<>* attr_value = propertyChild->first_attribute("value");

      if (attr_name == nullptr || attr_value == nullptr)
      {
        throwMissingAttribute(path, "property");
      }

      std::string name = attr_name->value();
      std::string type;

      if (arrayContext != nullptr)
      {
        ArrayFieldMap::iterator it = arrayContext->fields.find(name);
        if (it == arrayContext->fields.end())
        {
          propertyChild = propertyChild->next_sibling("property");
          continue;
        }

        type = it->second.type;
      }
      else
      {
        rapidxml::xml_attribute<>* attr_type = propertyChild->first_attribute("type");

        if (attr_type == nullptr)
        {
          throwMissingAttribute(path, "property", "type");
        }

        type = std::string(attr_type->value());
      }

      //bool isSubArray = type == "array";

      //if (!isSubArray)
      initializeProperty(path + name, type, std::string(attr_value->value()));

      AbstractPropertyValue::ValueMap::iterator it = m_values.find(path + name);
      if (arrayContext == nullptr)
      {
        group->addProperty(name, it->second);
      }
      else
      {
        arrayContext->propertyArray->addProperty(arrayContext->index, name, it->second);
      }

      propertyChild = propertyChild->next_sibling("property");
    }
  }

  void PropertiesManager::parseXmlArrayFields(rapidxml::xml_node<>* node, const std::string& path, ArrayContext* arrayContext)
  {
    rapidxml::xml_node<>* fieldChild = node->first_node("field");
    while (fieldChild != nullptr)
    {
      rapidxml::xml_attribute<>* attr_name = fieldChild->first_attribute("name");
      rapidxml::xml_attribute<>* attr_type = fieldChild->first_attribute("type");
      rapidxml::xml_attribute<>* attr_default = fieldChild->first_attribute("default");

      if (attr_name == nullptr || attr_type == nullptr)
      {
        throwMissingAttribute(path, "field");
      }

      std::string name = std::string(attr_name->value());
      std::string type = std::string(attr_type->value());

      if (arrayContext->fields.find(name) != arrayContext->fields.end())
      {
        throw GameException("Field " + name + " already exists at " + path);
      }

      ArrayField field;
      field.type = type;

      if (attr_default != nullptr)
      {
        field.defaultValue = std::string(attr_default->value());
      }

      arrayContext->fields.insert(ArrayFieldMap::value_type(name, field));

      fieldChild = fieldChild->next_sibling("field");
    }
  }

  void PropertiesManager::parseXmlArrayElements(rapidxml::xml_node<>* node, std::string path, ArrayContext* context)
  {
    int counter = 0;

    rapidxml::xml_node<>* elementChild = node->first_node("element");
    while (elementChild != nullptr)
    {
      char ac[16];
      sprintf(ac, "[%d]", counter);

      context->index = counter;

      std::string p = path + std::string(ac) + ".";

      parseXmlNode(elementChild, nullptr, p, context);

      for (ArrayFieldMap::iterator itfield = context->fields.begin(); itfield != context->fields.end(); ++itfield)
      {
        if (itfield->second.type == "array")
        {
          //printf("bla");
        }
        else
        {
          if (itfield->second.defaultValue != "" && !context->propertyArray->hasProperty(context->index, itfield->first))
          {
            initializeProperty(p + itfield->first, itfield->second.type, itfield->second.defaultValue);
            AbstractPropertyValue::ValueMap::iterator itprop = m_values.find(p + itfield->first);

            context->propertyArray->addProperty(context->index, itfield->first, itprop->second);
          }
        }
      }

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

}