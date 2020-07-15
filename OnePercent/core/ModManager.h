#pragma once

#include "scripting/LuaStateManager.h"

#include <osg/ref_ptr>
#include <osg/Referenced>

#include <osgGaming/Injector.h>

#include <QString>

namespace onep
{

  class ModManager : public osg::Referenced
  {
  public:
    using Ptr = osg::ref_ptr<ModManager>;

    explicit ModManager(osgGaming::Injector& injector);
    virtual ~ModManager();

    void loadModFromDirectory(const QString& path);
    void scanDirectoryForMods(const QString& path);

  private:
    LuaStateManager::Ptr m_lua;

    void addPathToLuaPackage(const QString& path);

  };

}
