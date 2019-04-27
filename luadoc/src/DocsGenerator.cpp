#include "luadoc/DocsGenerator.h"

#include <QTextStream>

struct StaticLibInitializer
{
  StaticLibInitializer()
  {
    Q_INIT_RESOURCE(luadoc_resources);
  }
};

StaticLibInitializer staticLibInitializer;

namespace luadoc
{
  enum class ScopeType
  {
    Namespace,
    Class
  };

  QString surroundByDivTag(const QString& text, const QString& id = "", const QString& className = "")
  {
    const auto& result = QString("<div%1%2>%3</div>")
                                 .arg(id.isEmpty() ? "" : QString(" id='%1'").arg(id))
                                 .arg(id.isEmpty() ? "" : QString(" class='%1'").arg(id))
                                 .arg(text);

    return result;
  }

  QString generateScopeHtml(ScopeType type, const DocsGenerator::ScopeDefPtr& scopePtr, const QString& content)
  {
    return surroundByDivTag(QString("<h3>%1 %2</h3>%3")
                                    .arg(type == ScopeType::Namespace ? "namespace" : "class")
                                    .arg(scopePtr->name)
                                    .arg(content),
                            "content");
  }

  QString getShortenedTypeName(const QString& typeName)
  {
    QString result = typeName;
    if (result.startsWith("class "))
    {
      result = result.right(result.length() - 6);
    }

    auto templateIndex = result.indexOf('<');
    if (templateIndex >= 0)
    {
      result = result.left(templateIndex);
    }

    auto nsIndex = result.lastIndexOf("::");
    if (nsIndex >= 0)
    {
      result = result.right(result.length() - nsIndex - 2);
    }

    auto spaceIndex = result.indexOf(' ');
    if (spaceIndex >= 0)
    {
      result = result.left(spaceIndex);
    }

    return result;
  }

  QString readFromFile(const QString& filename)
  {
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    return file.readAll();
  }

  bool writeToFile(const QString& filename, const QString& content)
  {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
      return false;
    }

    QTextStream stream(&file);
    stream << content;

    return true;
  }

  QString getRelativeClassFilePath(const DocsGenerator::ScopeDefPtr& source, const DocsGenerator::ScopeDefPtr& target)
  {
    auto depth = source->docsFilename.count('/') - 1;
    QString goUpPath;
    for (auto i=0; i<depth; i++)
    {
      goUpPath.append(QString("..%1").arg((i == (depth-1)) ? "" : "/"));
    }

    return (goUpPath.isEmpty() ? "." : goUpPath) + target->docsFilename;
  }

  bool needsDoubleQuotes(const QString& value)
  {
    return value.contains(';') || value.contains('"') || value.contains('\n');
  }

  QString stringToCsv(const QString& value)
  {
    auto result = value;
    result.replace("\"", "\"\"");

    if (needsDoubleQuotes(value))
    {
      result = QString("\"%1\"").arg(result);
    }

    return result;
  }

  QString csvToString(const QString& value)
  {
    auto result = value;

    if (needsDoubleQuotes(value))
    {
      auto len = result.length();
      result = result.left(len - 1).right(len - 2);
    }

    result.replace("\"\"", "\"");
    return result;
  }

  QStringList parseCsvRow(const QString& row)
  {
    QStringList values;
    auto elements = row.split(';');

    QString value;
    auto    previousWasConsistent = true;

    for (auto& element : elements)
    {
      element.replace("\"\"", "\"");
      auto len = element.length();

      if (previousWasConsistent)
      {
        auto hasDoubleQuotes = element.contains('"');

        if (!hasDoubleQuotes || (element.startsWith('"') && element.endsWith('"')))
        {
          if (hasDoubleQuotes)
          {
            element = element.left(len - 1).right(len - 2);
          }

          values.push_back(element);
        }
        else
        {
          previousWasConsistent = false;
          value = element.right(len - 1);
        }
      }
      else
      {
        if (!element.endsWith('"'))
        {
          value.append(';').append(element);
        }
        else
        {
          element = element.left(len - 1);
          values.push_back(value.append(';').append(element));
          previousWasConsistent = true;
        }
      }
    }

    return values;
  }

  QString makePageFromTemplate(const QString& content, int pathDepth)
  {
    QString stylesheetPath;
    for (auto i=0; i<pathDepth; i++)
    {
      stylesheetPath.append("../");
    }

    stylesheetPath.append("css/stylesheet.css");

    auto templateHtml = readFromFile(":/templates/content/template.html");
    templateHtml.replace("{CONTENT}", content);
    templateHtml.replace("{STYLESHEET_PATH}", stylesheetPath);

    return templateHtml;
  }

  DocsGenerator::FunctionDefinition::FunctionDefinition(const QString& n, const std::type_info& rt)
    : name(n)
    , returnType(rt)
  {

  }

  DocsGenerator::PropertyDefinition::PropertyDefinition(const QString& n, const std::type_info& t, bool ro)
    : name(n)
    , type(t)
    , isReadonly(ro)
  {
  }

  bool DocsGenerator::ScopeDefinition::hasAnyDeclarations() const
  {
    return !properties.empty() || !functions.empty();
  }

  bool DocsGenerator::ScopeDefinition::hasMember(const QString& name) const
  {
    return functions.count(name) || properties.count(name);
  }

  DocsGenerator& DocsGenerator::instance()
  {
    static DocsGenerator instance;
    return instance;
  }

  bool DocsGenerator::generate(const QString& projectName, const QString& outputPath, const QString& descriptionsFilename)
  {
    m_descriptions.clear();
    m_navigationReplaceHtml.clear();

    readDescriptionsFromCSV(descriptionsFilename);

    assert_return(!m_nsStack.empty(), false);

    QDir outputDir(outputPath);

    assert_return(outputDir.exists(), false);

    outputDir.mkdir("content");
    outputDir.mkdir("css");

    QString namespacesHtmlPath = "content/namespaces";

    assert_return(outputDir.exists("content"), false);
    assert_return(outputDir.exists("css"), false);

    assert_return(writeToFile(outputDir.filePath("index.html"), readFromFile(":/templates/index.html")), false);
    assert_return(writeToFile(outputDir.filePath("css/stylesheet.css"), readFromFile(":/templates/css/stylesheet.css")), false);

    traverseNamespace(*m_nsStack.cbegin(), QDir(outputDir.filePath(namespacesHtmlPath)));

    m_navigationReplaceHtml = surroundByDivTag(m_navigationReplaceHtml, "navigation");
    auto navigationHtml     = makePageFromTemplate(m_navigationReplaceHtml, 1);

    assert_return(writeToFile(outputDir.filePath("content/navigation.html"), navigationHtml), false);

    auto nameReplaceHtml = surroundByDivTag(QString("<h3>Lua API</h3><h2>%1</h2>").arg(projectName), "content");
    auto mainHtml        = makePageFromTemplate(nameReplaceHtml, 1);

    assert_return(writeToFile(outputDir.filePath("content/main.html"), mainHtml), false);
    assert_return(writeDescriptionsToCSV(descriptionsFilename), false);

    return true;
  }

  void DocsGenerator::beginNamespace(const QString& name)
  {
    auto path = getCurrentPath();
    auto ns   = std::make_shared<NamespaceDefinition>();
    ns->name  = name;

    if (!path.isEmpty())
    {
      ns->docsFilename = QString("%1%2.html").arg(path).arg(name);
    }

    const auto& currentNs = *m_nsStack.rbegin();

    currentNs->namespaces[ns->name] = ns;
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
  : m_idCounter(0)
  , m_nsStack({std::make_shared<NamespaceDefinition>()})
  {
  }

  bool DocsGenerator::writeDescriptionsToCSV(const QString& filename) const
  {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
      return false;
    }

    QTextStream stream(&file);
    stream << "Namespace;Class;Name;Short description;Description;State\n";

    for (const auto& description : m_descriptions)
    {
      QString stateStr;
      switch (description.second.state)
      {
      case Description::State::UNFINISHED:
        stateStr = "UNFINISHED";
        break;
      case Description::State::FINISHED:
        stateStr = "FINISHED";
        break;
      case Description::State::DEPRECATED:
        stateStr = "DEPRECATED";
        break;
      default:
        stateStr = "UNKNOWN_STATE";
        break;
      }

      auto ns = description.first.ns;
      ns.replace('/', '.');

      stream << QString("%1;%2;%3;%4;%5;%6\n")
        .arg(ns)
        .arg(description.first.cl)
        .arg(description.first.name)
        .arg(stringToCsv(description.second.shortDescription))
        .arg(stringToCsv(description.second.description))
        .arg(stateStr);
    }

    return true;
  }

  bool DocsGenerator::readDescriptionsFromCSV(const QString& filename)
  {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
      return false;
    }

    QTextStream stream(&file);
    stream.readLine();

    while (!stream.atEnd())
    {
      auto values = parseCsvRow(stream.readLine());

      assert_continue(values.size() == 6);

      auto ns = values[0];
      ns.replace('.', '/');

      DescriptionKey key;
      key.ns   = ns;
      key.cl   = values[1];
      key.name = values[2];

      Description desc;
      desc.shortDescription = values[3];
      desc.description      = values[4];

      if (isMemberForDescriptionExisting(key))
      {
        desc.state = (!desc.shortDescription.isEmpty() || !desc.description.isEmpty()) ? Description::State::FINISHED
                                                                                       : Description::State::UNFINISHED;
      }
      else
      {
        desc.state = Description::State::DEPRECATED;
      }

      m_descriptions[key] = desc;
    }

    return true;
  }

  QString DocsGenerator::getCurrentPath() const
  {
      QString path;
      for (const auto& ns : m_nsStack)
      {
        path.append(ns->name + "/");
      }

      return path;
  }

  bool DocsGenerator::isMemberForDescriptionExisting(const DescriptionKey& key) const
  {
    auto currentNamespace = *m_nsStack.cbegin();

    if (!key.ns.isEmpty())
    {
      auto namespaces = key.ns.split('/');

      for (const auto& ns : namespaces)
      {
        auto it = currentNamespace->namespaces.find(ns);
        if (it == currentNamespace->namespaces.end())
        { 
          return false;
        }

        currentNamespace = it->second;
      }
    }

    if (key.cl.isEmpty())
    {
      return currentNamespace->hasMember(key.name);
    }

    auto it = currentNamespace->classes.find(key.cl);
    if (it == currentNamespace->classes.end())
    {
      return false;
    }

    return it->second->hasMember(key.name);
  }

  const DocsGenerator::Description& DocsGenerator::getOrCreateDescription(const DescriptionKey& key)
  {
    auto it = m_descriptions.find(key);
    if (m_descriptions.count(key) == 0)
    {
      m_descriptions[key] = Description();
    }

    return m_descriptions[key];
  }

  QString DocsGenerator::tryGetHyperlinkOfScope(const ScopeDefPtr& scopePtr, const std::type_info& type) const
  {
    auto name = getShortenedTypeName(type.name()).toHtmlEscaped();

    const auto it = m_classes.find(type);
    if (it != m_classes.cend())
    {
      name = QString("<a href='%1' target='content'>%2</a>").arg(getRelativeClassFilePath(scopePtr, it->second)).arg(name);
    }

    return name;
  }

  QString DocsGenerator::generateScopeMembersHtml(const ScopeDefPtr& scopePtr, const QString& namespacePath,
                                                  const QString& head)
  {
    auto hasFunctions  = !scopePtr->functions.empty();
    auto hasProperties = !scopePtr->properties.empty();

    auto className = std::dynamic_pointer_cast<ClassDefinition>(scopePtr) ? scopePtr->name : "";

    QString classDescription = head;
    QString membersDescription;

    if (hasFunctions || hasProperties)
    {
      membersDescription.append("<h4>Detailed description</h4>");
    }

    auto memberCounter = 0;

    if (hasFunctions)
    {
      classDescription.append("<h4>Functions</h4>");
      classDescription.append("<table>");

      for (const auto& function : scopePtr->functions)
      {
        const auto& description = getOrCreateDescription({namespacePath, className, function.second->name});
        const auto  returnType  = tryGetHyperlinkOfScope(scopePtr, function.second->returnType);

        classDescription.append(
                QString("<tr><td><a href='#member_%1'>%2()</a> &rarr; %3</td><td>%4</td></tr>")
                        .arg(memberCounter)
                        .arg(function.second->name)
                        .arg(returnType)
                        .arg(description.shortDescription));

        membersDescription.append(
                QString("<p class='detailed_class' id='member_%1'>%2() &rarr; %3</p><p class='detailed_description'>%4</p>")
                        .arg(memberCounter)
                        .arg(function.second->name)
                        .arg(returnType)
                        .arg(description.description));

        memberCounter++;
      }

      classDescription.append("</table>");
    }

    if (hasProperties)
    {
      classDescription.append("<h4>Properties</h4>");
      classDescription.append("<table>");

      for (const auto& property : scopePtr->properties)
      {
        const auto& description   = getOrCreateDescription({namespacePath, className, property.second->name});
        const auto  returnType    = tryGetHyperlinkOfScope(scopePtr, property.second->type);
        const auto  accessibility = (property.second->isReadonly ? "R" : "RW");



        classDescription.append(QString("<tr><td><a href='#member_%1'>%2</a> :: %3 <i>[%4]</i></td><td>%5</td></tr>")
                                        .arg(memberCounter)
                                        .arg(property.second->name)
                                        .arg(returnType)
                                        .arg(accessibility)
                                        .arg(description.shortDescription));

        membersDescription.append(QString("<p class='detailed_class' id='member_%1'>%2 :: %3 <i>[%4]</i></p><p "
                                          "class='detailed_description'>%5</p>")
                                          .arg(memberCounter)
                                          .arg(property.second->name)
                                          .arg(returnType)
                                          .arg(accessibility)
                                          .arg(description.description));

        memberCounter++;
      }

      classDescription.append("</table>");
    }

    return classDescription.append(membersDescription);
  }

  void DocsGenerator::generateNamespace(const QDir& directory, const QString& namespacePath,
                                        const NamespaceDefPtr& namespacePtr)
  {
    auto namespaceDescription = generateScopeMembersHtml(namespacePtr, namespacePath);
    auto namespaceReplaceHtml = generateScopeHtml(ScopeType::Namespace, namespacePtr, namespaceDescription);

    auto pathDepth     = 2 + namespacePath.count('/');
    auto namespaceHtml = makePageFromTemplate(namespaceReplaceHtml, pathDepth);

    QFile namespaceFile(directory.path() + ".html");
    if (!namespaceFile.open(QIODevice::WriteOnly))
    {
      return;
    }

    QTextStream stream(&namespaceFile);
    stream << namespaceHtml;
  }

  void DocsGenerator::generateClass(const QDir& directory, const QString& namespacePath, const ClassDefPtr& classPtr)
  {
    QString classDescription;

    if (classPtr->baseClass)
    {
      classDescription.append(QString("<p><i>Inherits from</i> class <a href='%1' target='content'>%2</a></p>")
                                      .arg(getRelativeClassFilePath(classPtr, classPtr->baseClass), classPtr->baseClass->name));
    }

    classDescription      = generateScopeMembersHtml(classPtr, namespacePath, classDescription);
    auto classReplaceHtml = generateScopeHtml(ScopeType::Class, classPtr, classDescription);

    auto pathDepth = 2 + namespacePath.count('/') + (namespacePath.isEmpty() ? 0 : 1);
    auto classHtml = makePageFromTemplate(classReplaceHtml, pathDepth);

    auto filename = QFileInfo(classPtr->docsFilename).fileName();
    QFile classFile(directory.filePath(filename));
    if (!classFile.open(QIODevice::WriteOnly))
    {
      return;
    }

    QTextStream stream(&classFile);
    stream << classHtml;
  }

  void DocsGenerator::traverseNamespace(const NamespaceDefPtr& currentNamespace, const QDir& currentNamespaceDir,
                                        const QString& currentNamespacePath)
  {
    currentNamespaceDir.mkdir(".");

    auto isGlobalNs = currentNamespacePath.isEmpty();

    auto hasClasses      = !currentNamespace->classes.empty();
    auto hasDeclarations = currentNamespace->hasAnyDeclarations();

    if (hasClasses || hasDeclarations)
    {
      auto nsName = currentNamespacePath;
      nsName.replace('/', '.');

      if (isGlobalNs)
      {
        m_navigationReplaceHtml.append("<h4>Global namespace</h4>");
      }
      else
      {
        m_navigationReplaceHtml.append(QString("<a href='./namespaces/%1.html' target='content'><h4>%2</h4></a>")
                                               .arg(currentNamespacePath)
                                               .arg(nsName));

        generateNamespace(currentNamespaceDir, currentNamespacePath, currentNamespace);
      }

      if (hasClasses)
      {
        m_navigationReplaceHtml.append("<ul>");
        for (const auto& cl : currentNamespace->classes)
        {
          m_navigationReplaceHtml.append(
                  QString("<li><a href='./namespaces%1' target='content'>%2</a></li>").arg(cl.second->docsFilename).arg(cl.second->name));

          generateClass(currentNamespaceDir, currentNamespacePath, cl.second);
        }
        m_navigationReplaceHtml.append("</ul>");
      }
    }

    for (const auto& ns : currentNamespace->namespaces)
    {
      QDir nsDir(currentNamespaceDir.filePath(ns.second->name));

      traverseNamespace(ns.second, nsDir, currentNamespacePath + (isGlobalNs ? "" : "/") + ns.second->name);
    }
  }

  bool DocsGenerator::DescriptionKey::operator<(const DescriptionKey& other) const
  {
    auto nsCmp = ns.compare(other.ns);
    if (nsCmp == 0)
    {
      auto clCmp = cl.compare(other.cl);
      if (clCmp == 0)
      {
        return name.compare(other.name) < 0;
      }

      return clCmp < 0;
    }

    return nsCmp < 0;
  }
}
