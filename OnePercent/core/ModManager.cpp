#include "core/ModManager.h"
#include "core/Macros.h"

#include <QDir>

namespace onep
{

ModManager::ModManager(osgGaming::Injector& injector)
  : osg::Referenced()
  , m_lua(injector.inject<LuaStateManager>())
{
}

ModManager::~ModManager() = default;

void ModManager::loadModFromDirectory(const QString& path)
{
  OSGG_QLOG_INFO(QString("Loading scripts from directory %1").arg(path));

  QDir    dir(path);
  QString dataLuaFilename    = "data.lua";
  QString controlLuaFilename = "control.lua";

  const auto dataLuaFilepath    = dir.filePath(dataLuaFilename);
  const auto controlLuaFilepath = dir.filePath(controlLuaFilename);

  const auto dataScriptExists    = QFileInfo::exists(dataLuaFilepath);
  const auto controlScriptExists = QFileInfo::exists(controlLuaFilepath);

  if (!dataScriptExists && !controlScriptExists)
  {
    OSGG_QLOG_WARN(QString("Neither %1 nor %2 exist in the directory %3. No scripts were loaded")
                           .arg(dataLuaFilename)
                           .arg(controlLuaFilename)
                           .arg(path));
    return;
  }

  addPathToLuaPackage(dir.absolutePath());

  if (dataScriptExists)
  {
    m_lua->loadScript(dataLuaFilepath.toStdString());
  }

  if (controlScriptExists)
  {
      m_lua->loadScript(controlLuaFilepath.toStdString());
  }
}

void ModManager::scanDirectoryForMods(const QString& path)
{
  QDir dir(path);
  auto modDirs = dir.entryList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Dirs, QDir::SortFlag::Name);
  for (const auto& modDir : modDirs)
  {
    loadModFromDirectory(dir.filePath(modDir));
  }
}

void ModManager::addPathToLuaPackage(const QString& path)
{
  m_lua->safeExecute([this, path]()
  {
    luabridge::LuaRef packageRef  = m_lua->getGlobal("package");
    luabridge::LuaRef packagePath = packageRef["path"];

    if (!packagePath.isString())
    {
      OSGG_LOG_FATAL("Expected package.path to be a string");
      return;
    }

    auto packagePathStr = packagePath.tostring();
    packagePathStr.append(QString(";%1").arg(QDir::toNativeSeparators(QDir(path).filePath("?.lua"))).toStdString());

    packageRef["path"] = packagePathStr;
  });
}

}
