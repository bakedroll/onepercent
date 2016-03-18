#pragma once

#include <osg/Referenced>

#include <functional>
#include <vector>

namespace osgGaming
{
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

    void addNotifyFunc(NotifyFunc func)
    {
      m_notifyFuncs.push_back(func);
    }

    void addFuncAndNotify(NotifyFunc func)
    {
      m_notifyFuncs.push_back(func);
      func(m_value);
    }

    void set(T value)
    {
      m_value = value;

      for (typename NotifyFuncList::iterator it = m_notifyFuncs.begin(); it != m_notifyFuncs.end(); ++it)
        (*it)(m_value);
    }

    T get()
    {
      return m_value;
    }

  private:
    typedef std::vector<NotifyFunc> NotifyFuncList;

    T m_value;
    NotifyFuncList m_notifyFuncs;

  };
}