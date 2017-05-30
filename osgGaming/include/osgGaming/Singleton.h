#pragma once

#include <osg/Referenced>
#include <osg/ref_ptr>

namespace osgGaming
{
	template <class T>
	class Singleton : public osg::Referenced
	{
	public:
		static osg::ref_ptr<T> getInstance()
		{
			if (!_instance.valid())
			{
				_instance = new T();
			}

			return _instance;
		}

	private:
		static osg::ref_ptr<T> _instance;

	};

	template <class T>
	osg::ref_ptr<T> Singleton<T>::_instance;

}
