#pragma once

#include <osg/Referenced>

#include <functional>
#include <vector>

namespace osgGaming
{
  template<typename T>
  class Observer : public osg::Referenced
  {
  public:
    typedef osg::ref_ptr<Observer<T>> Ptr;
    typedef std::function<void(T)> NotifyFunc;

    Observer(NotifyFunc func) : execute(func) { }

    NotifyFunc execute;
  };

  template<>
  class Observer<void> : public osg::Referenced
  {
  public:
    typedef osg::ref_ptr<Observer> Ptr;
    typedef std::function<void()> NotifyFunc;

    Observer(NotifyFunc func) : execute(func) { }

    NotifyFunc execute;
  };

  template<typename T>
  class Observable : public osg::Referenced
  {
  public:
    typedef osg::ref_ptr<Observable<T>> Ptr;
    typedef std::function<void(T)> NotifyFunc;

    Observable()
    {
      
    }

    Observable(T initial)
      : m_value(initial)
    {
      
    }

    typename Observer<T>::Ptr connect(NotifyFunc func)
    {
      typename Observer<T>::Ptr observer = new Observer<T>(func);
      observers().push_back(observer);

      return observer;
    }

    typename Observer<T>::Ptr connectAndNotify(NotifyFunc func)
    {
      typename Observer<T>::Ptr observer = new Observer<T>(func);
      observers().push_back(observer);

      func(m_value);

      return observer;
    }

    void set(T value)
    {
      m_value = value;

      ObserverList& obs = observers();
      for (typename ObserverList::iterator it = obs.begin(); it != obs.end(); ++it)
        (*it)->execute(m_value);
    }

    T get()
    {
      return m_value;
    }

  protected:
    typedef std::vector<osg::observer_ptr<Observer<T>>> ObserverList;

    ObserverList& observers()
    {
      typename ObserverList::iterator it = m_observers.begin();
      while (it != m_observers.end())
      {
        typename Observer<T>::Ptr obs;
        if (!it->lock(obs))
          it = m_observers.erase(it);
        else
          ++it;
      }

      return m_observers;
    }

  private:
    T m_value;
    ObserverList m_observers;

  };
}