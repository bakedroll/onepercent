#pragma once

#include "scripting/LuaStateManager.h"

#include <osg/ref_ptr>
#include <osg/Referenced>

#include <osgHelper/ioc/Injector.h>

namespace onep
{

  class ModManager : public osg::Referenced
  {
  public:
    using Ptr = osg::ref_ptr<ModManager>;

    explicit ModManager(osgHelper::ioc::Injector& injector);
    virtual ~ModManager();

    void loadModFromDirectory(const QString& path);
    void scanDirectoryForMods(const QString& path);

  private:
    LuaStateManager::Ptr m_lua;

    void addPathToLuaPackage(const QString& path);

  };

}
