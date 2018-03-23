#pragma once

#include <osgGaming/PropertiesManager.h>

#include <osg/Referenced>

#include "core/Globals.h"

#include "ProgressingValue.h"
#include "ProgressingValueContainer.h"

namespace onep
{
  class SkillBranchContainer;

  class CountryValues : public osg::Referenced
  {
  public:
    typedef osg::ref_ptr<CountryValues> Ptr;

    CountryValues(osg::ref_ptr<osgGaming::PropertiesManager> propertiesManager, osg::ref_ptr<SkillBranchContainer> skillBranchContainer, float wealth);

    template<typename T>
    ProgressingValue<T>* getValue(CountryValueType type);

    template<typename T>
    ProgressingValue<T>* getBranchValue(CountryValueType type, int branchId);

    osg::ref_ptr<ProgressingValueContainer> getContainer();

  private:
    typedef std::map<int, ProgressingValueBase::Map> ProgressingValueBranchMap;

    int numBranches();
    std::string idToName(int id);

    ProgressingValueBase::Map m_values;
    ProgressingValueBranchMap m_branchValues;

    osg::ref_ptr<ProgressingValueContainer> m_container;

    osg::ref_ptr<SkillBranchContainer> m_skillBranchContainer;

    template<typename T>
    void createValue(CountryValueType type, T min, T max, T init);

    template<typename T>
    void createBranchValues(CountryValueType type, T min, T max, T init);

  };

  template <typename T>
  ProgressingValue<T>* CountryValues::getValue(CountryValueType type)
  {
    return dynamic_cast<ProgressingValue<T>*>(m_values.find(int(type))->second.get());
  }

  template <typename T>
  ProgressingValue<T>* CountryValues::getBranchValue(CountryValueType type, int branchId)
  {
    return dynamic_cast<ProgressingValue<T>*>(m_branchValues.find(branchId)->second.find(int(type))->second.get());
  }

  template <typename T>
  void CountryValues::createValue(CountryValueType type, T min, T max, T init)
  {
    osg::ref_ptr<ProgressingValue<T>> value = new ProgressingValue<T>(min, max, init);

    m_values.insert(ProgressingValueBase::Map::value_type(int(type), value));
    m_container->registerValue(value, countryValue_getStringFromType(int(type)));
  }

  template <typename T>
  void CountryValues::createBranchValues(CountryValueType type, T min, T max, T init)
  {
    int n = numBranches();
    for (int i = 0; i < n; i++)
    {
      osg::ref_ptr<ProgressingValue<T>> value = new ProgressingValue<T>(min, max, init);

      ProgressingValueBranchMap::iterator it = m_branchValues.find(i);
      if (it == m_branchValues.end())
      {
        ProgressingValueBase::Map values;
        values.insert(ProgressingValueBase::Map::value_type(int(type), value));
        m_branchValues.insert(ProgressingValueBranchMap::value_type(i, values));
      }
      else
      {
        it->second.insert(ProgressingValueBase::Map::value_type(int(type), value));
      }

      std::string name = idToName(i);
      m_container->registerValue(value, countryValue_getStringFromType(int(type)) + " [" + name + "]");
    }
  }
}