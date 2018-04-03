#include "simulation/CountryState.h"
#include "simulation/SkillBranch.h"
#include "core/Multithreading.h"

#include "QMutex"

namespace onep
{
  typedef std::map<QString, osgGaming::Observable<bool>::Ptr> BranchActivatedMap;

  struct CountryState::Impl
  {
    Impl() {}

    ValuesMap values;
    BranchValuesMap branchValues;

    std::string currentBranchName;

    BranchActivatedMap oActivatedBranches;

    QMutex mutexActivated;
  };

  CountryState::CountryState()
    : m(new Impl())
  {
  }

  CountryState::~CountryState()
  {
  }

  CountryState::Ptr CountryState::copy() const
  {
    Ptr c = new CountryState();

    for (ValuesMap::iterator it = m->values.begin(); it != m->values.end(); ++it)
      c->m->values[it->first] = new SimulatedLuaValue(it->second->get());

    for (BranchValuesMap::iterator it = m->branchValues.begin(); it != m->branchValues.end(); ++it)
      for (ValuesMap::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
        c->m->branchValues[it->first][vit->first] = new SimulatedLuaValue(vit->second->get());

    for (BranchActivatedMap::iterator it = m->oActivatedBranches.begin(); it != m->oActivatedBranches.end(); ++it)
      c->m->oActivatedBranches[it->first] = it->second;

    return c;
  }

  void CountryState::overwrite(Ptr other)
  {
    for (ValuesMap::iterator it = other->m->values.begin(); it != other->m->values.end(); ++it)
      m->values[it->first]->set(it->second->get());

    for (BranchValuesMap::iterator it = other->m->branchValues.begin(); it != other->m->branchValues.end(); ++it)
      for (ValuesMap::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
        m->branchValues[it->first][vit->first]->set(vit->second->get());
  }

  void CountryState::addValue(const char* name, float init)
  {
    m->values[name] = new SimulatedLuaValue(init);
  }

  void CountryState::addBranchValue(const char* name, SkillsContainer::Ptr skillsContainer, float init)
  {
    int n = skillsContainer->getNumBranches();
    for (int i = 0; i < n; i++)
    {
      std::string branchName = skillsContainer->getBranchByIndex(i)->getBranchName();
      m->branchValues[name][QString::fromStdString(branchName)] = new SimulatedLuaValue(init);
    }
  }

  CountryState::ValuesMap& CountryState::getValuesMap() const
  {
    return m->values;
  }

  CountryState::BranchValuesMap& CountryState::getBranchValuesMap() const
  {
    return m->branchValues;
  }

  bool CountryState::getBranchActivated(const char* branchName) const
  {
    QMutexLocker lock(&m->mutexActivated);
    return getOActivatedBranch(branchName)->get();
  }

  void CountryState::setBranchActivated(const char* branchName, bool activated)
  {
    QMutexLocker lock(&m->mutexActivated);
    getOActivatedBranch(branchName)->set(activated);
  }

  osgGaming::Observable<bool>::Ptr CountryState::getOActivatedBranch(const char* branchName) const
  {
    if (m->oActivatedBranches.count(QString(branchName)) == 0)
      m->oActivatedBranches.insert(BranchActivatedMap::value_type(QString(branchName), new osgGaming::Observable<bool>(false)));

    return m->oActivatedBranches[QString(branchName)];
  }

  void CountryState::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<CountryState>("CountryState")
      .addFunction("get_branch_value", &CountryState::lua_get_branch_value)
      .addFunction("get_value", &CountryState::lua_get_value)
      .addFunction("set_current_branch", &CountryState::lua_set_current_branch)
      .addFunction("get_branch_activated", &CountryState::lua_get_branch_activated)
      .addFunction("set_branch_activated", &CountryState::lua_set_branch_activated)
      .endClass();
  }

  SimulatedLuaValue* CountryState::lua_get_value(const char* name)
  {
    return m->values[name].get();
  }

  SimulatedLuaValue* CountryState::lua_get_branch_value(const char* name)
  {
    return m->branchValues[name][m->currentBranchName.c_str()].get();
  }

  void CountryState::lua_set_current_branch(std::string branchName)
  {
    m->currentBranchName = branchName;
  }

  bool CountryState::lua_get_branch_activated() const
  {
    return getBranchActivated(m->currentBranchName.c_str());
  }

  void CountryState::lua_set_branch_activated(bool activated)
  {
    std::string name = m->currentBranchName;

    Multithreading::uiExecuteOrAsync([=]()
    {
      setBranchActivated(name.c_str(), activated);
    });
  }
}