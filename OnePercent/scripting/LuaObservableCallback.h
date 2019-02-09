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
    {
      m_observer = observable->connect(osgGaming::Func<T>([this, &lua](T value)
      {
        trigger(lua, value);
      }));
    }

  private:
    typename osgGaming::Observer<T>::Ptr m_observer;
  };
}