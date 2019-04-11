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
  QString surroundByDivTag(const QString& text, const QString& id = "", const QString& className = "")
  {
    const auto& result = QString("<div%1%2>%3</div>")
                                 .arg(id.isEmpty() ? "" : QString(" id='%1'").arg(id))
                                 .arg(id.isEmpty() ? "" : QString(" class='%1'").arg(id))
                                 .arg(text);

    return result;
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

  QString getRelativeClassFilePath(const DocsGenerator::ClassDefPtr& source, const DocsGenerator::ClassDefPtr& target)
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
    auto ns  = std::make_shared<NamespaceDefinition>();
    ns->name = name;

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
    auto namespaces       = key.ns.split('/');

    for (const auto& ns : namespaces)
    {
      auto it = currentNamespace->namespaces.find(ns);
      if (it == currentNamespace->namespaces.end())
      {
        return false;
      }

      currentNamespace = it->second;
    }

    if (key.cl.isEmpty())
    {
      return false;
    }

    auto it = currentNamespace->classes.find(key.cl);
    if (it == currentNamespace->classes.end())
    {
      return false;
    }

    return it->second->functions.count(key.name) || it->second->properties.count(key.name);
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

  void DocsGenerator::generateClass(const QDir& directory, const QString& namespacePath, const ClassDefPtr& classPtr)
  {
    auto hasFunctions  = !classPtr->functions.empty();
    auto hasProperties = !classPtr->properties.empty();

    QString classDescription;
    QString membersDescription;

    if (classPtr->baseClass)
    {
      classDescription.append(QString("<p><i>Inherits from</i> class <a href='%1' target='content'>%2</a></p>")
                                      .arg(getRelativeClassFilePath(classPtr, classPtr->baseClass), classPtr->baseClass->name));
    }

    if (hasFunctions || hasProperties)
    {
      membersDescription.append("<h4>Detailed description</h4>");
    }

    auto memberCounter = 0;

    if (hasFunctions)
    {
      classDescription.append("<h4>Functions</h4>");
      classDescription.append("<table>");

      for (const auto& function : classPtr->functions)
      {
        const auto& description = getOrCreateDescription({namespacePath, classPtr->name, function.second.name});
        const auto  returnType  = getShortenedTypeName(function.second.returnType).toHtmlEscaped();

        classDescription.append(
                QString("<tr><td><a href='#member_%1'>%2()</a> &rarr; %3</td><td>%4</td></tr>")
                        .arg(memberCounter)
                        .arg(function.second.name)
                        .arg(returnType)
                        .arg(description.shortDescription));

        membersDescription.append(
                QString("<p class='detailed_class' id='member_%1'>%2() &rarr; %3</p><p class='detailed_description'>%4</p>")
                        .arg(memberCounter)
                        .arg(function.second.name)
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

      for (const auto& property : classPtr->properties)
      {
        const auto& description   = getOrCreateDescription({namespacePath, classPtr->name, property.second.name});
        const auto  returnType    = getShortenedTypeName(property.second.type).toHtmlEscaped();
        const auto  accessibility = (property.second.isReadonly ? "R" : "RW");

        classDescription.append(QString("<tr><td><a href='#member_%1'>%2</a> :: %3 <i>[%4]</i></td><td>%5</td></tr>")
                                        .arg(memberCounter)
                                        .arg(property.second.name)
                                        .arg(returnType)
                                        .arg(accessibility)
                                        .arg(description.shortDescription));

        membersDescription.append(QString("<p class='detailed_class' id='member_%1'>%2 :: %3 <i>[%4]</i></p><p "
                                          "class='detailed_description'>%5</p>")
                                          .arg(memberCounter)
                                          .arg(property.second.name)
                                          .arg(returnType)
                                          .arg(accessibility)
                                          .arg(description.shortDescription));

        memberCounter++;
      }

      classDescription.append("</table>");
    }

    auto classReplaceHtml = surroundByDivTag(
            QString("<h3>class %1</h3>%2%3").arg(classPtr->name).arg(classDescription).arg(membersDescription),
            "content");


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

    if (!currentNamespace->classes.empty())
    {
      auto nsName = currentNamespacePath;
      nsName.replace('/', '.');

      m_navigationReplaceHtml.append(QString("<h4>%1</h4>").arg(isGlobalNs ? "Global namespace" : nsName));

      m_navigationReplaceHtml.append("<ul>");
      for (const auto& cl : currentNamespace->classes)
      {
        m_navigationReplaceHtml.append(
                QString("<li><a href='./namespaces%1' target='content'>%2</a></li>").arg(cl.second->docsFilename).arg(cl.second->name));

        generateClass(currentNamespaceDir, currentNamespacePath, cl.second);
      }
      m_navigationReplaceHtml.append("</ul>");
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
