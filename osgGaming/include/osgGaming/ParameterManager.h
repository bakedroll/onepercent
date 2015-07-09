#pragma once

#include <string>
#include <map>

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

	private:
		typedef std::map<std::string, osg::ref_ptr<AbstractParameterValue>> ValueMap;

		ValueMap _values;
	};
}