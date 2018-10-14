#include "simulation/ModelContainer.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaStateManager.h"
#include "core/Enums.h"

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

  void ModelContainer::initializeLuaModelData()
  {
    m->model->traverseElements(static_cast<int>(ModelTraversalType::INITIALIZE_DATA));
  }

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

  }
}