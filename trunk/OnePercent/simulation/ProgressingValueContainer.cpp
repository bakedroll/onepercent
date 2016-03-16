#include "ProgressingValueContainer.h"

namespace onep
{
  void ProgressingValueContainer::registerValue(osg::ref_ptr<ProgressingValueBase> value, std::string name)
  {
    m_values[name] = value;
  }

  void ProgressingValueContainer::step()
  {
    for (ProgressingValuesMap::iterator it = m_values.begin(); it != m_values.end(); ++it)
      it->second->step();
  }

  void ProgressingValueContainer::debugPrintToString(std::string& str)
  {
    for (ProgressingValuesMap::iterator it = m_values.begin(); it != m_values.end(); ++it)
    {
      str += it->first + ": ";
      it->second->debugPrintToString(str);
    }
  }
}