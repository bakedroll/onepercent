#include <osgGaming/ResourceManager.h>
#include <osgGaming/TextResource.h>
#include <osgGaming/Helper.h>
#include <osgGaming/BinaryResource.h>
#include <osgGaming/GameException.h>

#include <osgDB/ReadFile>
#include <osgDB/FileNameUtils>

using namespace osg;
using namespace osgDB;
using namespace std;
using namespace osgGaming;
using namespace osgText;

ref_ptr<ResourceManager> ResourceManager::getInstance()
{
	if (!_instance.valid())
	{
		_instance = new ResourceManager();
	}

	return _instance;
}

string ResourceManager::loadText(string resourceKey)
{
	return static_cast<TextResource*>(loadObject(resourceKey, TEXT).get())->text;
}

char* ResourceManager::loadBinary(std::string resourceKey)
{
	return static_cast<BinaryResource*>(loadObject(resourceKey, BINARY).get())->getBytes();
}

ref_ptr<Image> ResourceManager::loadImage(string resourceKey)
{
	return static_cast<Image*>(loadObject(resourceKey).get());
}

ref_ptr<Font> ResourceManager::loadFont(string resourceKey)
{
	return static_cast<Font*>(loadObject(resourceKey).get());
}

ref_ptr<Shader> ResourceManager::loadShader(string resourceKey, Shader::Type type)
{
	return static_cast<Shader*>(loadObject(resourceKey, SHADER, type).get());
}

ref_ptr<Font> ResourceManager::loadDefaultFont()
{
	if (_defaultFontResourceKey == "")
	{
		return NULL;
	}

	return loadFont(_defaultFontResourceKey);
}

void ResourceManager::setDefaultFontResourceKey(string resourceKey)
{
	_defaultFontResourceKey = resourceKey;
}

void ResourceManager::setResourceLoader(ref_ptr<ResourceLoader> loader)
{
	_resourceLoader = loader;
}

void ResourceManager::clearCacheResource(string resourceKey)
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

ResourceManager::ResourceManager()
	: Referenced(),
	  _defaultFontResourceKey("")
{

}

ref_ptr<ResourceManager> ResourceManager::_instance = NULL;

ref_ptr<ResourceLoader> ResourceManager::resourceLoader()
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

string ResourceManager::loadTextFromStream(std::ifstream& stream, long long length)
{
	char* buffer = new char[length + 1];
	stream.read(buffer, length);
	buffer[length] = '\0';

	string text = string(buffer);

	delete[] buffer;

	return text;
}

ref_ptr<Object> ResourceManager::loadObject(string resourceKey, ResourceType type, Shader::Type shaderType)
{
	ref_ptr<Object> obj = getCacheItem(resourceKey);
	if (obj.valid())
	{
		return obj;
	}

	std::ifstream stream;
	long long length;
	resourceLoader()->getResourceStream(resourceKey, stream, length);

	if (type == DETECT)
	{
		ref_ptr<ReaderWriter> rw = Registry::instance()->getReaderWriterForExtension(getLowerCaseFileExtension(resourceKey));
		ReaderWriter::ReadResult res = rw->readObject(stream);

		obj = res.getObject();
	}
	else if (type == TEXT)
	{
		ref_ptr<TextResource> textRes = new TextResource();
		textRes->text = loadTextFromStream(stream, length);

		obj = textRes;
	}
	else if (type == BINARY)
	{
		ref_ptr<BinaryResource> binRes = new BinaryResource();
		binRes->setBytes(loadBytesFromStream(stream, length));

		obj = binRes;
	}
	else if (type == SHADER)
	{
		ref_ptr<Shader> shader = new Shader(shaderType);
		string source = loadTextFromStream(stream, length);

		shader->setShaderSource(source);

		obj = shader;
	}

	stream.close();

	storeCacheItem(resourceKey, obj);

	return obj;
}

ref_ptr<Object> ResourceManager::getCacheItem(string key)
{
	string lower_key = lowerString(key);

	ResourceDictionary::iterator it = _cache.find(lower_key);
	if (it == _cache.end())
	{
		return NULL;
	}

	return it->second;
}

void ResourceManager::storeCacheItem(string key, ref_ptr<Object> obj)
{
	string lower_key = lowerString(key);

	_cache.insert(ResourceDictionary::value_type(lower_key, obj));
}