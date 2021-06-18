#pragma once

#include <osgHelper/Macros.h>

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
      using Ptr = std::shared_ptr<FunctionDefinition>;

      FunctionDefinition(const QString& n, const std::type_info& rt);

      QString               name;
      const std::type_info& returnType;
    };

    struct PropertyDefinition
    {
      using Ptr = std::shared_ptr<PropertyDefinition>;

      PropertyDefinition(const QString& n, const std::type_info& t, bool ro);

      QString               name;
      const std::type_info& type;
      bool                  isReadonly;
    };

    struct ScopeDefinition
    {
      virtual ~ScopeDefinition() = default;
      bool hasAnyDeclarations() const;
      bool hasMember(const QString& name) const;

      QString                                    name;
      QString                                    docsFilename;
      std::map<QString, FunctionDefinition::Ptr> functions;
      std::map<QString, PropertyDefinition::Ptr> properties;
    };

    using ScopeDefPtr = std::shared_ptr<ScopeDefinition>;

    struct ClassDefinition : ScopeDefinition
    {
      int         id;
      ClassDefPtr baseClass;
      QString     returnType;
    };

    struct NamespaceDefinition;
    using NamespaceDefPtr = std::shared_ptr<NamespaceDefinition>;

    struct NamespaceDefinition : ScopeDefinition
    {
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
    void addScopeFunction(const ScopeDefPtr& scope, const QString& name) const
    {
      assert_return(scope);
      const auto& type = typeid(typename luabridge::detail::FuncTraits<MemFn>::ReturnType);
      scope->functions[name] = std::make_shared<FunctionDefinition>(name, type);
    }

    template <typename ReturnType>
    void addScopeProperty(const ScopeDefPtr& scope, const QString& name, bool isReadonly) const
    {
      assert_return(scope);
      const auto& type = typeid(ReturnType);
      scope->properties[name] = std::make_shared<PropertyDefinition>(name, type, isReadonly);
    }

    template <typename MemFn>
    void addCurrentClassFunction(const QString& name)
    {
      addScopeFunction<MemFn>(m_currentClass, name);
    }

    template <typename ReturnType>
    void addCurrentClassProperty(const QString& name, bool isReadonly)
    {
      addScopeProperty<ReturnType>(m_currentClass, name, isReadonly);
    }

    template <typename MemFn>
    void addCurrentNamespaceFunction(const QString& name)
    {
      addScopeFunction<MemFn>(*m_nsStack.rbegin(), name);
    }

    template <typename ReturnType>
    void addCurrentNamespaceProperty(const QString& name, bool isReadonly)
    {
      addScopeProperty<ReturnType>(*m_nsStack.rbegin(), name, isReadonly);
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
    QString            tryGetHyperlinkOfScope(const ScopeDefPtr& classPtr, const std::type_info& type) const;

    QString generateScopeMembersHtml(const ScopeDefPtr& scopePtr, const QString& namespacePath, const QString& head = "");

    void generateNamespace(const QDir& directory, const QString& namespacePath, const NamespaceDefPtr& namespacePtr);
    void generateClass(const QDir& directory, const QString& namespacePath, const ClassDefPtr& classPtr);

    template <typename T>
    ClassDefPtr createClassDefinition(const QString& name)
    {
      const auto& currentNs  = *m_nsStack.rbegin();

      ClassDefPtr classDef;

      if (!currentNs->classes.count(name))
      {
        m_idCounter++;
        auto path = getCurrentPath();

        classDef               = std::make_shared<ClassDefinition>();
        classDef->id           = m_idCounter;
        classDef->docsFilename = QDir(path).filePath(QString("%1.html").arg(m_idCounter));
        classDef->name         = name;

        currentNs->classes[name] = classDef;
        m_currentClass           = classDef;
        m_classes[typeid(T)]     = classDef;
        m_classes[typeid(T*)]    = classDef;
      }
      else
      {
        classDef = currentNs->classes[name];
      }

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
