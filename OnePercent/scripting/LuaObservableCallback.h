#pragma once

#include "scripting/LuaCallback.h"

#include <osgGaming/Observable.h>

namespace onep
{
  template <typename T>
  class LuaObservableCallback : public LuaCallback
  {
  public:
    LuaObservableCallback(const LuaStateManager::Ptr& lua, const typename osgGaming::Observable<T>::Ptr& observable)
      : m_oldValue(observable->get())
    {
      m_observer = observable->connect([this, &lua](T value)
      {
        trigger(lua, m_oldValue, value);
        m_oldValue = value;
      });
    }

  private:
    typename osgGaming::Observer<T>::Ptr m_observer;
    T                                    m_oldValue;

  };
}
