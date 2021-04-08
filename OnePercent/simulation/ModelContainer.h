#pragma once

#include <osg/Referenced>
#include <osgHelper/ioc/Injector.h>

#include <functional>
#include <vector>

namespace onep
{
  class LuaModel;

  class ModelContainer : public osg::Referenced
  {
  public:
    using Ptr = osg::ref_ptr<ModelContainer>;

    ModelContainer(osgHelper::ioc::Injector& injector);
    ~ModelContainer();

    std::shared_ptr<LuaModel> getModel() const;

    void accessModel(std::function<void(const std::shared_ptr<LuaModel>&)> func) const;

    void initializeState();
    void initializeCountryNeighbours(const std::map<int, std::vector<int>>& neighbourships);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}