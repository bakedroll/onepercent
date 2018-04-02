#include "Skill.h"

namespace onep
{
  struct Skill::Impl
  {
    Impl()
      : obActivated(new osgGaming::Observable<bool>(false))
    {}

    std::string name;
    std::string displayName;
    std::string type;
    int cost;

    osgGaming::Observable<bool>::Ptr obActivated;

    std::vector<std::unique_ptr<luabridge::LuaRef>> actions;
  };

  Skill::Skill(std::string name)
    : Node()
    , SimulationCallback()
    , m(new Impl())
  {
    m->name = name;

    setUpdateCallback(new Callback());
  }

  Skill::Skill(std::string name, std::string displayName, std::string type, int cost)
    : Node()
    , SimulationCallback()
    , m(new Impl())
  {
    m->name = name;
    m->displayName = displayName;
    m->type = type;
    m->cost = cost;

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

  void Skill::addBranchAttribute(int branchId, CountryValueType valueType, ProgressingValueMethod method, float value)
  {
    BranchAttribute branchAttribute;
    branchAttribute.branchId = branchId;
    branchAttribute.attribute.method = method;
    branchAttribute.attribute.value = value;
    branchAttribute.attribute.valueType = valueType;

    m_branchAttributes.push_back(branchAttribute);
  }

  std::string Skill::getName()
  {
    return m->name;
  }

  osgGaming::Observable<bool>::Ptr Skill::getObActivated() const
  {
    return m->obActivated;
  }

  bool Skill::callback(SimulationVisitor* visitor)
  {
    if (!m->obActivated->get())
      return false;

    for (Attribute::List::iterator it = m_attributes.begin(); it != m_attributes.end(); ++it)
      visitor->getCountryValues()->getValue<float>(it->valueType)->prepare(it->value, it->method);

    for (BranchAttribute::List::iterator it = m_branchAttributes.begin(); it != m_branchAttributes.end(); ++it)
      visitor->getCountryValues()->getBranchValue<float>(it->attribute.valueType, it->branchId)->prepare(it->attribute.value, it->attribute.method);

    return false;
  }

  void Skill::onAction()
  {
    for (std::vector<std::unique_ptr<luabridge::LuaRef>>::iterator it = m->actions.begin(); it != m->actions.end(); ++it)
    {
      (*it->get())(1);
    }
  }

  void Skill::addOnAction(luabridge::LuaRef& luaRef)
  {
    m->actions.push_back(std::make_unique<luabridge::LuaRef>(luaRef));
  }
}