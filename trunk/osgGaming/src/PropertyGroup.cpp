#include <osgGaming/PropertyGroup.h>

using namespace std;
using namespace osg;
using namespace osgGaming;

ref_ptr<PropertyGroup> PropertyGroup::group(string name)
{
	return _groups.find(name)->second;
}

ref_ptr<PropertyArray> PropertyGroup::array(string name)
{
	return _arrays.find(name)->second;
}

void PropertyGroup::addGroup(std::string name, osg::ref_ptr<PropertyGroup> group)
{
	if (_groups.find(name) == _groups.end())
	{
		_groups.insert(GroupMap::value_type(name, group));
	}
}

void PropertyGroup::addArray(std::string name, osg::ref_ptr<PropertyArray> a)
{
	if (_arrays.find(name) == _arrays.end())
	{
		_arrays.insert(ArrayMap::value_type(name, a));
	}
}

void PropertyGroup::addProperty(std::string name, osg::ref_ptr<AbstractPropertyValue> value)
{
	if (_properties.find(name) == _properties.end())
	{
		_properties.insert(PropertyMap::value_type(name, value));
	}
}