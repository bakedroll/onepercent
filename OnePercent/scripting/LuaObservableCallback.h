#pragma once

#include "scripting/LuaCallback.h"

#include <osgHelper/Observable.h>

namespace onep
{
  template <typename T>
  class LuaObservableCallback : public LuaCallback
  {
  public:
    LuaObservableCallback(const LuaStateManager::Ptr& lua, const typename osgHelper::Observable<T>::Ptr& observable)
      : m_oldValue(observable->get())
    {
      m_observer = observable->connect([this, &lua](T value)
      {
        trigger(lua, m_oldValue, value);
        m_oldValue = value;
      });
    }

  private:
    typename osgHelper::Observer<T>::Ptr m_observer;
    T                                    m_oldValue;

  };
}
