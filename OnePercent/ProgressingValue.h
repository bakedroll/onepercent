#pragma once

#include <vector>

#include <osg/Referenced>

namespace onep
{
  class ProgressingValueBase : public osg::Referenced
  {
  public:
    typedef std::vector<osg::ref_ptr<ProgressingValueBase>> List;

    virtual void step() = 0;
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

  private:
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
    void registerValue(osg::ref_ptr<ProgressingValueBase> value)
    {
      m_values.push_back(value);
    }

    void step()
    {
      for (ProgressingValueBase::List::iterator it = m_values.begin(); it != m_values.end(); ++it)
        it->get()->step();
    }

  private:
    ProgressingValueBase::List m_values;
  };
}