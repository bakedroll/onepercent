#pragma once

#include <vector>

#include <osgGaming/Timer.h>
#include <osgGaming/Singleton.h>

namespace osgGaming
{
	class TimerFactory : public Singleton<TimerFactory>
	{
	public:
		TimerFactory();

		osg::ref_ptr<Timer> create(void(*callback)(), double duration = 1.0, bool singleShot = true);

		template <class T>
		osg::ref_ptr<Timer> create(void(T::*callback)(), T* context, double duration = 1.0, bool singleShot = true)
		{
			osg::ref_ptr<Timer> timer = new Timer(std::bind(callback, context), duration, singleShot);

			_timers.push_back(timer);

			return timer;
		}

		void updateRegisteredTimers(double time);

	private:
		typedef std::vector<osg::ref_ptr<Timer>> TimerList;

		TimerList _timers;
	};
}