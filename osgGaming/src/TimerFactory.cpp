#include <osgGaming/TimerFactory.h>

using namespace osgGaming;
using namespace osg;

template<>
ref_ptr<TimerFactory> Singleton<TimerFactory>::_instance;

TimerFactory::TimerFactory()
{

}

ref_ptr<Timer> TimerFactory::create(void(*callback)(), double duration, bool singleShot)
{
	ref_ptr<Timer> timer = new Timer(std::bind(callback), duration, singleShot);

	_timers.push_back(timer);

	return timer;
}

void TimerFactory::updateRegisteredTimers(double time)
{
	for (TimerList::iterator it = _timers.begin(); it != _timers.end(); )
	{
		if (it->get()->update(time))
		{
			if (it->get()->referenceCount() == 1)
			{
				it = _timers.erase(it);
				continue;
			}
		}

		++it;
	}
}
