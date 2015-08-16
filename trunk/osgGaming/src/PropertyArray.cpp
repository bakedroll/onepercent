#include <osgGaming/PropertyArray.h>

using namespace std;
using namespace osg;
using namespace osgGaming;

int PropertyArray::size()
{
	return _array.size();
}

void PropertyArray::addProperty(int index, string name, ref_ptr<AbstractPropertyValue> value)
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

bool PropertyArray::hasProperty(int index, string name)
{
	PropertyArrayMap::iterator itarr = _array.find(index);

	if (itarr == _array.end())
	{
		return false;
	}

	if (itarr->second.find(name) == itarr->second.end())
	{
		return false;
	}

	return true;
}