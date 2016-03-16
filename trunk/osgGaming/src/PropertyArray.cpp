#include <osgGaming/PropertyArray.h>

using namespace std;
using namespace osg;
using namespace osgGaming;

int PropertyArray::size()
{
	return m_propertyArray.size();
}

void PropertyArray::addProperty(int index, string name, ref_ptr<AbstractPropertyValue> value)
{
	PropertyArrayMap::iterator itarr = m_propertyArray.find(index);

	if (itarr == m_propertyArray.end())
	{
		PropertyNameValueMap pnvm;
		pnvm.insert(PropertyNameValueMap::value_type(name, value));
		m_propertyArray.insert(PropertyArrayMap::value_type(index, pnvm));

		return;
	}
	
	PropertyNameValueMap::iterator itval = itarr->second.find(name);
	if (itval == itarr->second.end())
		itarr->second.insert(PropertyNameValueMap::value_type(name, value));
}

void PropertyArray::addArray(int index, std::string name, osg::ref_ptr<PropertyArray> arr)
{
  ArrayArrayMap::iterator itarr = m_arrayArray.find(index);

  if (itarr == m_arrayArray.end())
  {
    ArrayNameArrayMap anam;
    anam.insert(ArrayNameArrayMap::value_type(name, arr));
    m_arrayArray.insert(ArrayArrayMap::value_type(index, anam));

    return;
  }

  ArrayNameArrayMap::iterator itval = itarr->second.find(name);
  if (itval == itarr->second.end())
    itarr->second.insert(ArrayNameArrayMap::value_type(name, arr));
}

bool PropertyArray::hasProperty(int index, string name)
{
	PropertyArrayMap::iterator itarr = m_propertyArray.find(index);

	if (itarr == m_propertyArray.end())
		return false;

	if (itarr->second.find(name) == itarr->second.end())
		return false;

	return true;
}

bool PropertyArray::hasArray(int index, std::string name)
{
  ArrayArrayMap::iterator itarr = m_arrayArray.find(index);

  if (itarr == m_arrayArray.end())
    return false;

  if (itarr->second.find(name) == itarr->second.end())
    return false;

  return true;
}

osg::ref_ptr<PropertyArray> PropertyArray::array(int index, std::string name)
{
  return m_arrayArray.find(index)->second.find(name)->second;
}