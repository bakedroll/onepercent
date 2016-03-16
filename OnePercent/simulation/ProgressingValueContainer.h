#pragma once

#include <map>

#include <osg/Referenced>
#include <osg/ref_ptr>

#include "ProgressingValue.h"

namespace onep
{
  class ProgressingValueContainer : public osg::Referenced
  {
  public:
    void registerValue(osg::ref_ptr<ProgressingValueBase> value, std::string name);
    void step();

    void debugPrintToString(std::string& str);

  private:
    typedef std::map<std::string, osg::ref_ptr<ProgressingValueBase>> ProgressingValuesMap;

    ProgressingValuesMap m_values;
  };
}