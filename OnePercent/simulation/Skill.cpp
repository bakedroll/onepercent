#include "Skill.h"

namespace onep
{

  Skill::Skill(std::string name)
    : Node()
    , m_name(name)
    , m_activated(false)
  {
    setUpdateCallback(new Callback());
  }

  void Skill::addAttribute(CountryValueType valueType, ProgressingValueMethod method, float value)
  {
    Attribute attribute;
    attribute.method = method;
    attribute.value = value;
    attribute.valueType = valueType;

    m_attributes.push_back(attribute);
  }

  void Skill::addBranchAttribute(BranchType branch, CountryValueType valueType, ProgressingValueMethod method, float value)
  {
    BranchAttribute branchAttribute;
    branchAttribute.branchType = branch;
    branchAttribute.attribute.method = method;
    branchAttribute.attribute.value = value;
    branchAttribute.attribute.valueType = valueType;

    m_branchAttributes.push_back(branchAttribute);
  }

  void Skill::setActivated(bool activated)
  {
    m_activated = activated;
  }

  std::string Skill::getName()
  {
    return m_name;
  }

  bool Skill::getActivated()
  {
    return m_activated;
  }

  bool Skill::callback(SimulationVisitor* visitor)
  {
    if (!m_activated)
      return false;

    for (Attribute::List::iterator it = m_attributes.begin(); it != m_attributes.end(); ++it)
      visitor->getCountryValues()->getValue<float>(it->valueType)->prepare(it->value, it->method);

    for (BranchAttribute::List::iterator it = m_branchAttributes.begin(); it != m_branchAttributes.end(); ++it)
      visitor->getCountryValues()->getBranchValue<float>(it->attribute.valueType, it->branchType)->prepare(it->attribute.value, it->attribute.method);

    return false;
  }

}