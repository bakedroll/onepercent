#pragma once

#include <vector>
#include <map>

#include <osg/Referenced>

namespace onep
{
  class ProgressingValueBase : public osg::Referenced
  {
  public:
    typedef std::vector<osg::ref_ptr<ProgressingValueBase>> List;

    virtual void step() = 0;
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
      , m_change(init)
      , m_value(init)
      , m_init(init)
    {
    }

    virtual void step() override
    {
      m_value += (m_balance + m_influence);
      m_value = osg::clampBetween<T>(m_value, m_min, m_max);

      m_influence = m_init;
      m_change = m_init;
    }

    void setBalance(T balance)
    {
      m_balance = balance;
    }

    void setValue(T value)
    {
      m_value = value;
    }

    void addInfluence(T influence)
    {
      m_influence += influence;
    }

    void addChange(T change)
    {
      m_change = change;
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
      return m_balance;
    }

    T getValue()
    {
      return m_value + m_change;
    }

    virtual void debugPrintToString(std::string& str) override
    {
      T percent = m_value * static_cast<T>(100) / m_max;

      str += 
        round(std::to_string(m_value), 2) + "/" + round(std::to_string(m_max), 2) +
        " [" + round(std::to_string(percent), 2) + "%]\n";
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
    T m_change;
    T m_value;
    T m_init;
  };

  class ProgressingValueContainer : public osg::Referenced
  {
  public:
    void registerValue(osg::ref_ptr<ProgressingValueBase> value, std::string name)
    {
      m_values[name] = value;
    }

    void step()
    {
      for (ProgressingValuesMap::iterator it = m_values.begin(); it != m_values.end(); ++it)
        it->second->step();
    }

    void debugPrintToString(std::string& str)
    {
      for (ProgressingValuesMap::iterator it = m_values.begin(); it != m_values.end(); ++it)
      {
        str += it->first + ": ";
        it->second->debugPrintToString(str);
      }
    }

  private:
    typedef std::map<std::string, osg::ref_ptr<ProgressingValueBase>> ProgressingValuesMap;

    ProgressingValuesMap m_values;
  };
}