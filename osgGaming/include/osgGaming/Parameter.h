#pragma once

#include <string>

#include <osgGaming/ParameterManager.h>

namespace osgGaming
{
	template <typename T, char const* N>
	class Parameter
	{
	public:
		Parameter()
		{
			_name = N;
			_value = ParameterManager::getInstance()->getValuePtr<T>(_name);
		}

		void operator =(T value)
		{
			*_value = value;
		}

		T operator ~()
		{
			return *_value;
		}

	private:
		std::string _name;
		T* _value;

	};
}