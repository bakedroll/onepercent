#include "simulation/ModelContainer.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaStateManager.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaValuesDefTable.h"
#include "scripting/LuaValueDef.h"
#include "scripting/LuaCountry.h"

#include <QMutex>

namespace onep
{
  struct ModelContainer::Impl
  {
    Impl(osgGaming::Injector& injector)
      : lua(injector.inject<LuaStateManager>())
    {}

    LuaStateManager::Ptr lua;
    LuaModel::Ptr model;

    QMutex mutexModel;
  };

  ModelContainer::ModelContainer(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl(injector))
  {
    m->model = m->lua->createElement<LuaModel>("model");
  }

  ModelContainer::~ModelContainer() = default;

  std::shared_ptr<LuaModel> ModelContainer::getModel() const
  {
    return m->model;
  }

  void ModelContainer::accessModel(std::function<void(std::shared_ptr<LuaModel>)> func) const
  {
    QMutexLocker lock(&m->mutexModel);
    func(m->model);
  }

  void ModelContainer::initializeState()
  {
    QMutexLocker lock(&m->mutexModel);

    auto stateTable     = m->model->getSimulationStateTable();
    auto branchesTable  = m->model->getBranchesTable();
    auto countriesTable = m->model->getCountriesTable();
    auto valuesTable    = m->model->getValuesDefTable();

    countriesTable->foreachMappedElementDo<LuaCountry>([&](LuaCountry::Ptr country)
    {
      auto cid = country->getId();

      stateTable->addCountryState(country->getId());
      auto state = stateTable->getCountryState(cid);

      branchesTable->foreachMappedElementDo<LuaSkillBranch>([&](LuaSkillBranch::Ptr& branch)
      {
        auto name = branch->getBranchName();
        state->getBranchesActivatedTable()->addBranchActivated(name);
        state->getBranchValuesTable()->addBranch(name);
      });

      valuesTable->foreachMappedElementDo<LuaValueDef>([&](LuaValueDef::Ptr& def)
      {
        auto name = def->getName();
        auto init = country->getInitValue(name, def->getInit());

        switch (def->getType())
        {
          case LuaValueDef::Type::Default:
          {
            state->getValuesTable()->setValue(name, init);
            break;
          }
          case LuaValueDef::Type::Branch:
          {
            branchesTable->foreachMappedElementDo<LuaSkillBranch>([&](LuaSkillBranch::Ptr& branch)
            {
              state->getBranchValuesTable()->getBranch(branch->getBranchName())->setValue(name, init);
            });
            break;
          }
          default:
          {
            assert(false);
            break;
          }
        }
      });
    });
  }

  void ModelContainer::initializeCountryNeighbours(const std::map<int, std::vector<int>>& neighbourships)
  {
    auto statesTable = m->model->getSimulationStateTable();

    for (auto& neighbourship : neighbourships)
    {
      auto cid  = neighbourship.first;
      auto nids = neighbourship.second;

      for (auto nid : nids)
      {
        luabridge::LuaRef stateRef = statesTable->getCountryState(nid)->luaref();
        statesTable->getCountryState(cid)->addNeighbourState(nid, stateRef);
      }
    }
  }
}