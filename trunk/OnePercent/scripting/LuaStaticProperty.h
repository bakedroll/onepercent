#pragma once

namespace onep
{
  template <typename T>
  class LuaStaticProperty
  {
  public:
    static T* get()
    {
      return m_value;
    }

    static void set(T* value)
    {
      m_value = value;
    }

  private:
    static T* m_value;
  };

  template <typename T>
  T* LuaStaticProperty<T>::m_value = nullptr;
}