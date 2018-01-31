#include <osgGaming/TimerFactory.h>

namespace osgGaming
{

  TimerFactory::TimerFactory(Injector& injector)
  {
  }

  TimerFactory::~TimerFactory()
  {
  }

  osg::ref_ptr<Timer> TimerFactory::create(void(*callback)(), double duration, bool singleShot)
  {
    osg::ref_ptr<Timer> timer = new Timer(std::bind(callback), duration, singleShot);

    m_timers.push_back(timer);

    return timer;
  }

  void TimerFactory::updateRegisteredTimers(double time)
  {
    for (TimerList::iterator it = m_timers.begin(); it != m_timers.end();)
    {
      if (it->get()->update(time))
      {
        if (it->get()->referenceCount() == 1)
        {
          it = m_timers.erase(it);
          continue;
        }
      }

      ++it;
    }
  }

}