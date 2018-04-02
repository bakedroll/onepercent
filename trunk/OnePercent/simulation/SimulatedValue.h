#pragma once

#include "core/LuaStateManager.h"

#include <osg/Referenced>

namespace onep
{
  template <typename T>
  class SimulatedValue : public osg::Referenced
  {
  public:
    SimulatedValue(const T& initial)
      : m_value(initial)
    {}

    ~SimulatedValue() {}

    virtual const T& get() const { return m_value; }

    virtual void set(const T& value) { m_value = value; }

  protected:
    T m_value;
  };
}