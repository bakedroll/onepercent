#include "luadoc/DocsGenerator.h"

namespace luadoc
{
  DocsGenerator& DocsGenerator::instance()
  {
    static DocsGenerator instance;
    return instance;
  }

  void DocsGenerator::beginNamespace(const QString& name)
  {
    auto ns  = std::make_shared<NamespaceDefinition>();
    ns->name = name;

    const auto& currentNs = *m_nsStack.rbegin();

    currentNs->namespaces.push_back(ns);
    m_nsStack.push_back(ns);
  }

  void DocsGenerator::endNamespace()
  {
    m_nsStack.pop_back();
  }

  void DocsGenerator::endClass()
  {

    m_currentClass.reset();
  }

  DocsGenerator::DocsGenerator()
    : m_nsStack({ std::make_shared<NamespaceDefinition>() })
  {
    
  }
}
