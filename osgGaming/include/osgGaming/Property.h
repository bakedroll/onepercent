#pragma once

#include <string>

#include <osgGaming/PropertiesManager.h>
#include <osgGaming/GameException.h>

namespace osgGaming
{
	template <typename T, char const* N = nullptr>
	class Property
	{
	public:
		Property()
		{
			if (N == nullptr)
			{
				throw GameException("Invalid parameter key");
			}

			_name = N;
			_value = PropertiesManager::getInstance()->getValuePtr<T>(_name);
		}

		Property(const char* key)
		{
			_name = key;
			_value = PropertiesManager::getInstance()->getValuePtr<T>(_name);
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