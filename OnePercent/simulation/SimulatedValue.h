#pragma once

#include <osg/Referenced>
#include <osg/Math>

namespace onep
{
  template <typename T>
  class SimulatedValue : public osg::Referenced
  {
  public:
    SimulatedValue(const T& initial, const T& min, const T& max)
      : m_value(initial)
      , m_min(min)
      , m_max(max)
    {}

    ~SimulatedValue() {}

    virtual const T& get() const { return m_value; }

    virtual void set(const T& value) { m_value = osg::clampBetween<T>(value, m_min, m_max); }

  protected:
    T m_value;
    T m_min;
    T m_max;

  };
}