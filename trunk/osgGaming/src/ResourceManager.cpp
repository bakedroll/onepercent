#include <osgGaming/ResourceManager.h>

#include <osgGaming/Helper.h>

#include <osgDB/ReadFile>
#include <osgDB/FileNameUtils>

using namespace osg;
using namespace std;
using namespace osgGaming;
using namespace osgDB;
using namespace osgText;

ref_ptr<ResourceManager> ResourceManager::getInstance()
{
	if (!_instance.valid())
	{
		_instance = new ResourceManager();
	}

	return _instance;
}

ref_ptr<Image> ResourceManager::loadImage(string resourceKey)
{
	return static_cast<Image*>(loadObject(resourceKey).get());
}

ref_ptr<Font> ResourceManager::loadFont(string resourceKey)
{
	return static_cast<Font*>(loadObject(resourceKey).get());
}

ref_ptr<Shader> ResourceManager::loadShader(string resourceKey, osg::Shader::Type type)
{
	ref_ptr<Object> obj = getCacheItem(resourceKey);
	if (obj.valid())
	{
		return static_cast<Shader*>(obj.get());
	}

	ref_ptr<Shader> shader = new Shader(type);
	shader->loadShaderSourceFromFile(resourceKey);

	storeCacheItem(resourceKey, shader);

	return shader;
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

ResourceManager::ResourceManager()
	: Referenced(),
	  _defaultFontResourceKey("")
{

}

ref_ptr<ResourceManager> ResourceManager::_instance = NULL;

ref_ptr<Object> ResourceManager::loadObject(string resourceKey)
{
	ref_ptr<Object> obj = getCacheItem(resourceKey);
	if (obj.valid())
	{
		return obj;
	}

	ref_ptr<ReaderWriter> rw = Registry::instance()->getReaderWriterForExtension(getLowerCaseFileExtension(resourceKey));
	ReaderWriter::ReadResult res = rw->readObject(resourceKey);

	obj = res.getObject();

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