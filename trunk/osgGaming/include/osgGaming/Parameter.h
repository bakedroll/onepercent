#pragma once

#include <string>

#include <osgGaming/ParameterManager.h>
#include <osgGaming/GameException.h>

namespace osgGaming
{
	template <typename T, char const* N = nullptr>
	class Parameter
	{
	public:
		Parameter()
		{
			if (N == nullptr)
			{
				throw GameException("Invalid parameter key");
			}

			_name = N;
			_value = ParameterManager::getInstance()->getValuePtr<T>(_name);
		}

		Parameter(const char* key)
		{
			_name = key;
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