#include <osgGaming/PropertyArray.h>

using namespace osgGaming;

int PropertyArray::size()
{
	return _array.size();
}

void PropertyArray::addProperty(int index, std::string name, osg::ref_ptr<AbstractPropertyValue> value)
{
	PropertyArrayMap::iterator itarr = _array.find(index);

	if (itarr == _array.end())
	{
		PropertyNameValueMap pnvm;
		pnvm.insert(PropertyNameValueMap::value_type(name, value));
		_array.insert(PropertyArrayMap::value_type(index, pnvm));

		return;
	}
	
	PropertyNameValueMap::iterator itval = itarr->second.find(name);
	if (itval == itarr->second.end())
	{
		itarr->second.insert(PropertyNameValueMap::value_type(name, value));
	}
}