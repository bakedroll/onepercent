#pragma once

#include <osgGaming/Macros.h>

#include <QDir>
#include <QString>

#include <vector>
#include <memory>
#include <map>
#include <cassert>
#include <typeindex>

class QDir;

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge/LuaBridge.h>

namespace luadoc
{
  class DocsGenerator
  {
  public:
    struct ClassDefinition;
    using ClassDefPtr = std::shared_ptr<ClassDefinition>;

    struct FunctionDefinition
    {
      QString name;
      QString returnType;
    };

    struct PropertyDefinition
    {
      QString name;
      QString type;
      bool    isReadonly;
    };

    struct ClassDefinition
    {
      int         id;
      QString     docsFilename;
      QString     name;
      ClassDefPtr baseClass;
      QString     returnType;

      std::map<QString, FunctionDefinition> functions;
      std::map<QString, PropertyDefinition> properties;
    };

    struct NamespaceDefinition;
    using NamespaceDefPtr = std::shared_ptr<NamespaceDefinition>;

    struct NamespaceDefinition
    {
      QString                            name;
      std::map<QString, NamespaceDefPtr> namespaces;
      std::map<QString, ClassDefPtr>     classes;
    };

    DocsGenerator(const DocsGenerator& other)  = delete;
    DocsGenerator(const DocsGenerator&& other) = delete;

    static DocsGenerator& instance();

    bool generate(const QString& projectName, const QString& outputPath, const QString& descriptionsFilename);

    void beginNamespace(const QString& name);
    void endNamespace();

    template <typename T>
    void beginClass(char const* name)
    {
      createClassDefinition<T>(name);
    }

    template <typename T, typename U>
    void deriveClass(char const* name)
    {
      auto classDef = createClassDefinition<T>(name);

      const auto& it = m_classes.find(typeid(U));
      if (it == m_classes.end())
      {
        assert(false);
        return;
      }

      classDef->baseClass = it->second;
    }

    void endClass();

    template <typename MemFn>
    void addCurrentClassFunction(const QString& name)
    {
      assert_return(m_currentClass);
      const auto& type = typeid(luabridge::FuncTraits<MemFn>::ReturnType);
      m_currentClass->functions[name] = { name, type.name() };
    }

    template <typename ReturnType>
    void addCurrentClassProperty(const QString& name, bool isReadonly)
    {
      assert_return(m_currentClass);
      const auto& type = typeid(ReturnType);
      m_currentClass->properties[name] = { name, type.name(), isReadonly };
    }

  private:
    struct DescriptionKey
    {
      QString ns;
      QString cl;
      QString name;

      bool operator<(const DescriptionKey& other) const;
    };

    struct Description
    {
      enum class State
      {
        FINISHED,
        UNFINISHED,
        DEPRECATED
      };

      State   state = State::UNFINISHED;
      QString shortDescription;
      QString description;
    };

    using DescriptionsTable = std::map<DescriptionKey, Description>;

    DocsGenerator();

    bool writeDescriptionsToCSV(const QString& filename) const;
    bool readDescriptionsFromCSV(const QString& filename);

    QString            getCurrentPath() const;
    bool               isMemberForDescriptionExisting(const DescriptionKey& key) const;
    const Description& getOrCreateDescription(const DescriptionKey& key);

    void generateClass(const QDir& directory, const QString& namespacePath, const ClassDefPtr& classPtr);

    template <typename T>
    ClassDefPtr createClassDefinition(const QString& name)
    {
      m_idCounter++;

      auto path = getCurrentPath();

      const auto& currentNs  = *m_nsStack.rbegin();
      auto        classDef   = std::make_shared<ClassDefinition>();
      classDef->id           = m_idCounter;
      classDef->docsFilename = QDir(path).filePath(QString("%1.html").arg(m_idCounter));
      classDef->name         = name;

      currentNs->classes[name] = classDef;
      m_currentClass           = classDef;
      m_classes[typeid(T)]     = classDef;

      return classDef;
    }

    void traverseNamespace(const NamespaceDefPtr& currentNamespace, const QDir& currentNamespaceDir,
                           const QString& currentNamespacePath = "");

    int m_idCounter;

    std::vector<NamespaceDefPtr> m_nsStack;
    ClassDefPtr                  m_currentClass;

    std::map<std::type_index, ClassDefPtr> m_classes;

    DescriptionsTable m_descriptions;
    QString           m_navigationReplaceHtml;
  };
}
