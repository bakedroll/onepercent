#include <osgGaming/ResourceManager.h>
#include <osgGaming/TextResource.h>
#include <osgGaming/Helper.h>
#include <osgGaming/BinaryResource.h>
#include <osgGaming/GameException.h>

#include <osgDB/ReadFile>
#include <osgDB/FileNameUtils>

namespace osgGaming
{

  ResourceManager::ResourceManager(Injector& injector)
  {
    // _utf8Locale = locale(locale(), new codecvt_utf8<char>);
  }

  ResourceManager::~ResourceManager()
  {
  }

  std::string ResourceManager::loadText(std::string resourceKey)
  {
    return static_cast<TextResource*>(loadObject(resourceKey, TEXT).get())->text;
  }

  char* ResourceManager::loadBinary(std::string resourceKey)
  {
    return static_cast<BinaryResource*>(loadObject(resourceKey, BINARY).get())->getBytes();
  }

  osg::ref_ptr<osg::Image> ResourceManager::loadImage(std::string resourceKey)
  {
    return static_cast<osg::Image*>(loadObject(resourceKey).get());
  }

  osg::ref_ptr<osgText::Font> ResourceManager::loadFont(std::string resourceKey)
  {
    return static_cast<osgText::Font*>(loadObject(resourceKey).get());
  }

  osg::ref_ptr<osg::Shader> ResourceManager::loadShader(std::string resourceKey, osg::Shader::Type type)
  {
    return static_cast<osg::Shader*>(loadObject(resourceKey, SHADER, type).get());
  }

  void ResourceManager::setResourceLoader(osg::ref_ptr<ResourceLoader> loader)
  {
    _resourceLoader = loader;
  }

  void ResourceManager::clearCacheResource(std::string resourceKey)
  {
    ResourceDictionary::iterator it = _cache.find(lowerString(resourceKey));
    if (it == _cache.end())
    {
      throw GameException("Clear cache resource: resource key '" + resourceKey + "' not found");
    }

    _cache.erase(it);
  }

  void ResourceManager::clearCache()
  {
    _cache.clear();
  }

  void ResourceManager::setDefaultFont(const osg::ref_ptr<osgText::Font>& font)
  {
      m_defaultFont = font;
  }

  osg::ref_ptr<osgText::Font> ResourceManager::getDefaultFont()
  {
      return m_defaultFont;
  }

  osg::ref_ptr<ResourceLoader> ResourceManager::resourceLoader()
  {
    if (!_resourceLoader.valid())
    {
      _resourceLoader = new FileResourceLoader();
    }

    return _resourceLoader;
  }

  char* ResourceManager::loadBytesFromStream(std::ifstream& stream, long long length)
  {
    char* buffer = new char[length];
    stream.read(buffer, length);

    return buffer;
  }

  std::string ResourceManager::loadTextFromStream(std::ifstream& stream, long long length)
  {
    char* buffer = new char[length + 1];
    stream.read(buffer, length);
    buffer[length] = '\0';

    std::string text = std::string(buffer);

    delete[] buffer;

    return text;
  }

  osg::ref_ptr<osg::Object> ResourceManager::loadObject(std::string resourceKey, ResourceType type, osg::Shader::Type shaderType)
  {
    osg::ref_ptr<osg::Object> obj = getCacheItem(resourceKey);
    if (obj.valid())
    {
      return obj;
    }

    std::ifstream stream;

    long long length;
    resourceLoader()->getResourceStream(resourceKey, stream, length);

    if (type == DETECT)
    {
      osg::ref_ptr<osgDB::ReaderWriter> rw = osgDB::Registry::instance()->getReaderWriterForExtension(osgDB::getLowerCaseFileExtension(resourceKey));
      osgDB::ReaderWriter::ReadResult res = rw->readObject(stream);

      obj = res.getObject();
    }
    else if (type == TEXT)
    {
      // stream.imbue(_utf8Locale);

      osg::ref_ptr<TextResource> textRes = new TextResource();
      textRes->text = loadTextFromStream(stream, length);

      obj = textRes;
    }
    else if (type == BINARY)
    {
      osg::ref_ptr<BinaryResource> binRes = new BinaryResource();
      binRes->setBytes(loadBytesFromStream(stream, length));

      obj = binRes;
    }
    else if (type == SHADER)
    {
      osg::ref_ptr<osg::Shader> shader = new osg::Shader(shaderType);
      std::string source = loadTextFromStream(stream, length);

      shader->setShaderSource(source);

      obj = shader;
    }

    stream.close();

    storeCacheItem(resourceKey, obj);

    return obj;
  }

  osg::ref_ptr<osg::Object> ResourceManager::getCacheItem(std::string key)
  {
    std::string lower_key = lowerString(key);

    ResourceDictionary::iterator it = _cache.find(lower_key);
    if (it == _cache.end())
      return nullptr;

    return it->second;
  }

  void ResourceManager::storeCacheItem(std::string key, osg::ref_ptr<osg::Object> obj)
  {
    std::string lower_key = lowerString(key);

    _cache.insert(ResourceDictionary::value_type(lower_key, obj));
  }

  osg::ref_ptr<osgText::Font> ResourceManager::m_defaultFont;

}