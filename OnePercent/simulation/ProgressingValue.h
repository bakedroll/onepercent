#pragma once

#include "core/Globals.h"

#include <vector>
#include <map>
#include <string>
#include <assert.h>

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osg/Math>

namespace onep
{
  class ProgressingValueBase : public osg::Referenced
  {
  public:
    typedef std::vector<osg::ref_ptr<ProgressingValueBase>> List;
    typedef std::map<int, osg::ref_ptr<ProgressingValueBase>> Map;

    virtual void step() = 0;
    virtual bool full() = 0;
    virtual void debugPrintToString(std::string& str) = 0;
  };

  template<typename T>
  class ProgressingValue : public ProgressingValueBase
  {
  public:
    typedef osg::ref_ptr<ProgressingValue<T>> Ptr;

    ProgressingValue(T min, T max, T init)
      : m_min(min)
      , m_max(max)
      , m_balance(init)
      , m_influence(init)
      , m_lastInfluence(init)
      , m_change(init)
      , m_lastChange(init)
      , m_value(init)
      , m_init(init)
    {
    }

    virtual void step() override
    {
      m_value += (m_balance + m_influence);
      m_value = osg::clampBetween<T>(m_value, m_min - m_change, m_max - m_change);

      m_lastChange = m_change;
      m_lastInfluence = m_influence;

      m_influence = m_init;
      m_change = m_init;
    }

    virtual bool full() override
    {
      return getValue() >= m_max;
    }

    void prepare(T value, ProgressingValueMethod method)
    {
      switch (method)
      {
      case METHOD_SET_BALANCE:
        m_balance = value;
        break;
      case METHOD_SET_VALUE:
        m_value = value;
        break;
      case METHOD_ADD_INFLUENCE:
        m_influence += value;
        break;
      case METHOD_ADD_CHANGE:
        m_change += value;
        break;
      case METHOD_UNDEFINED:
      default:
        assert(false);
        break;
      }
    }

    T getMin()
    {
      return m_min;
    }

    T getMax()
    {
      return m_max;
    }

    T getBalance()
    {
      return m_balance + m_influence;
    }

    T getValue()
    {
      return m_value + m_change;
    }

    virtual void debugPrintToString(std::string& str) override
    {
      T value = m_value + m_lastChange;
      T balance = m_balance + m_lastInfluence;
      T percent = value * static_cast<T>(100) / m_max;

      str += 
        round(std::to_string(value), 2) + "/" + round(std::to_string(m_max), 2) +
        " [" + round(std::to_string(percent), 2) + "% | " + (balance >= 0 ? std::string("+") : std::string("")) +
        round(std::to_string(balance), 2) + "]\n";
    }

  private:

    std::string round(const std::string& str, int n)
    {
      std::size_t pos = str.find('.');
      if (pos == std::string::npos || str.size() < (pos + 1 + n))
        return str;

      return str.substr(0, pos + 1 + n);
    }

    T m_min;
    T m_max;
    T m_balance;
    T m_influence;
    T m_lastInfluence;
    T m_change;
    T m_lastChange;
    T m_value;
    T m_init;
  };

}