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
  QString getShortenedTypeName(const QString& typeName)
  {
    QString result = typeName;
    if (result.startsWith("class "))
    {
      result = result.right(result.length() - 6);
    }

    auto index = result.indexOf('<');
    if (index >= 0)
    {
      result = result.left(index).append("<...>");
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

  QString getRelativePath(const DocsGenerator::ClassDefPtr& source, const DocsGenerator::ClassDefPtr& target)
  {
    auto depth = source->docsFilename.count('/') - 1;
    QString goUpPath;
    for (auto i=0; i<depth; i++)
    {
      goUpPath.append(QString("..%1").arg((i == (depth-1)) ? "" : "/"));
    }

    if (goUpPath.isEmpty())
    {
      return target->docsFilename.right(target->docsFilename.length() - 1);
    }

    return goUpPath + target->docsFilename;
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

    assert_return(outputDir.exists("content"), false);
    assert_return(outputDir.exists("css"), false);

    assert_return(writeToFile(outputDir.filePath("index.html"), readFromFile(":/templates/index.html")), false);
    assert_return(writeToFile(outputDir.filePath("css/stylesheet.css"), readFromFile(":/templates/css/stylesheet.css")), false);

    auto navigationHtml = readFromFile(":/templates/content/navigation.html");

    traverseNamespace(*m_nsStack.cbegin(), QDir(outputDir.filePath("content/namespaces")));

    navigationHtml.replace("{NAVIGATION_LIST}", m_navigationReplaceHtml);
    m_navigationReplaceHtml.clear();

    assert_return(writeToFile(outputDir.filePath("content/navigation.html"), navigationHtml), false);

    auto mainHtml = readFromFile(":/templates/content/main.html");
    mainHtml.replace("{PROJECT_NAME}", projectName);

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
    auto classHtml = readFromFile(":/templates/content/class.html");
    if (classHtml.isEmpty())
    {
      return;
    }

    QString classDescription;
    if (classPtr->baseClass)
    {
      classDescription.append(QString("<p><i>Inherits from</i> class <a href='%1' target='content'>%2</a></p>")
                                      .arg(getRelativePath(classPtr, classPtr->baseClass), classPtr->baseClass->name));
    }

    if (!classPtr->functions.empty())
    {
      classDescription.append("<h4>Functions</h4>");
      classDescription.append("<table>");

      for (const auto& function : classPtr->functions)
      {
        const auto& description = getOrCreateDescription({ namespacePath, classPtr->name, function.second.name });

        classDescription.append(QString("<tr><td>%1() &rarr; %2</td><td>%3</td></tr>")
                                        .arg(function.second.name)
                                        .arg(getShortenedTypeName(function.second.returnType).toHtmlEscaped())
                                        .arg(description.shortDescription));
      }

      classDescription.append("</table>");
    }

    if (!classPtr->properties.empty())
    {
      classDescription.append("<h4>Properties</h4>");
      classDescription.append("<table>");

      for (const auto& property : classPtr->properties)
      {
        const auto& description = getOrCreateDescription({ namespacePath, classPtr->name, property.second.name });

        classDescription.append(QString("<tr><td>%1 :: %2 <i>[%3]</i></td><td>%4</td></tr>")
                                        .arg(property.second.name)
                                        .arg(getShortenedTypeName(property.second.type).toHtmlEscaped())
                                        .arg(property.second.isReadonly ? "R" : "RW")
                                        .arg(description.shortDescription));
      }

      classDescription.append("</table>");
    }

    classHtml.replace("{CLASS_NAME}", classPtr->name);
    classHtml.replace("{CLASS_DESCRIPTION}", classDescription);

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
                QString("<li><a href='namespaces%1' target='content'>%2</a></li>").arg(cl.second->docsFilename).arg(cl.second->name));

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
