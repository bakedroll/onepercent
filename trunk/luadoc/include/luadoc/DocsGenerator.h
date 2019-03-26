#pragma once

#include <osgGaming/Macros.h>

#include <QString>

#include <vector>
#include <memory>
#include <map>
#include <cassert>
#include <typeindex>

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
      QString     name;
      ClassDefPtr baseClass;
      QString     returnType;

      std::vector<FunctionDefinition> functions;
      std::vector<PropertyDefinition> properties;
    };

    struct NamespaceDefinition;
    using NamespaceDefPtr = std::shared_ptr<NamespaceDefinition>;

    struct NamespaceDefinition
    {
      QString                      name;
      std::vector<NamespaceDefPtr> namespaces;
      std::vector<ClassDefPtr>     classes;
    };

    DocsGenerator(const DocsGenerator& other)  = delete;
    DocsGenerator(const DocsGenerator&& other) = delete;

    static DocsGenerator& instance();

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
      m_currentClass->functions.push_back({ name, type.name() });
    }

    template <typename ReturnType>
    void addCurrentClassProperty(const QString& name, bool isReadonly)
    {
      assert_return(m_currentClass);
      const auto& type = typeid(ReturnType);
      m_currentClass->properties.push_back({ name, type.name(), isReadonly });
    }

  private:
    DocsGenerator();

    template <typename T>
    ClassDefPtr createClassDefinition(const QString& name)
    {
      const auto& currentNs = *m_nsStack.rbegin();
      auto        classDef  = std::make_shared<ClassDefinition>();
      classDef->name        = name;

      currentNs->classes.push_back(classDef);
      m_currentClass       = classDef;
      m_classes[typeid(T)] = classDef;

      return classDef;
    }

    std::vector<NamespaceDefPtr> m_nsStack;
    ClassDefPtr                  m_currentClass;

    std::map<std::type_index, ClassDefPtr> m_classes;
  };
}
