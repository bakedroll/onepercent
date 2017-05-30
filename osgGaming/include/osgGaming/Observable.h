#pragma once

#include <osg/Referenced>
#include <osg/observer_ptr>

#include <functional>
#include <vector>

namespace osgGaming
{
  template<typename T>
  class Func
  {
  public:
    Func(const std::function<void(T)>& func)
      : m_func(func)
    {
    }

    void exec(T arg) { m_func(arg); }

  private:
    std::function<void(T)> m_func;
  };

  template<>
  class Func<void>
  {
  public:
    Func(const std::function<void()>& func)
      : m_func(func)
    {
    }

    void exec() { m_func(); }

  private:
    std::function<void()> m_func;
  };


  template<typename T>
  class Observer : public osg::Referenced
  {
  public:
    typedef osg::ref_ptr<Observer<T>> Ptr;

    Observer(const Func<T>& func) : func(func) { }

    Func<T> func;
  };

  template<>
  class Observer<void> : public osg::Referenced
  {
  public:
    typedef osg::ref_ptr<Observer> Ptr;

    Observer(const Func<void>& func) : func(func) { }

    Func<void> func;
  };

  namespace __ObservableInternals
  {
    template<typename T>
    class ObservableBase : public osg::Referenced
    {
    public:
      ObservableBase() : osg::Referenced() {}

      typename Observer<T>::Ptr connect(const Func<T>& func)
      {
        typename Observer<T>::Ptr observer = new Observer<T>(func);
        observers().push_back(observer);

        return observer;
      }

      typename Observer<T>::Ptr connectAndNotify(const Func<T>& func)
      {
        typename Observer<T>::Ptr observer = connect(func);
        notify(observer);

        return observer;
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

      virtual void notify(typename Observer<T>::Ptr obs) = 0;

      void notifyAll()
      {
        ObserverList& obs = observers();
        for (typename ObserverList::iterator it = obs.begin(); it != obs.end(); ++it)
          notify(*it);
      }

    //private:
      ObserverList m_observers;
    };
  }

  template<typename T>
  class Observable : public __ObservableInternals::ObservableBase<T>
  {
  public:
	virtual ~Observable() {}

    typedef osg::ref_ptr<Observable<T>> Ptr;

    Observable()
      : __ObservableInternals::ObservableBase<T>()
    {
    }

    Observable(T initial)
      : __ObservableInternals::ObservableBase<T>()
      , m_value(initial)
    {
    }

    void set(T value)
    {
      m_value = value;
      __ObservableInternals::ObservableBase<T>::notifyAll();
    }

    T get()
    {
      return m_value;
    }

  protected:

    virtual void notify(typename Observer<T>::Ptr obs) override
    {
      obs->func.exec(m_value);
    }

  private:
    T m_value;

  };

  class Signal : public __ObservableInternals::ObservableBase<void>
  {
  public:
    Signal()
      : __ObservableInternals::ObservableBase<void>()
    {
    }

    void trigger()
    {
      notifyAll();
    }

  protected:
    virtual void notify(Observer<void>::Ptr obs) override
    {
      obs->func.exec();
    }

  };
}
