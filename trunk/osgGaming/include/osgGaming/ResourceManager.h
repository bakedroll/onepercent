#pragma once

#include <map>
#include <fstream>

#include <osgGaming/FileResourceLoader.h>
#include <osgGaming/Singleton.h>

#include <osg/Image>
#include <osgText/Font>
#include <osgDB/ReadFile>
#include <osg/Shader>

namespace osgGaming
{
	class ResourceManager : public Singleton<ResourceManager>
	{
	public:
		typedef enum _resourceType
		{
			DETECT,
			TEXT,
			BINARY,
			SHADER
		} ResourceType;

		ResourceManager();

		std::string loadText(std::string resourceKey);
		char* loadBinary(std::string resourceKey);
		osg::ref_ptr<osg::Image> loadImage(std::string resourceKey);
		osg::ref_ptr<osgText::Font> loadFont(std::string resourceKey);
		osg::ref_ptr<osg::Shader> loadShader(std::string resourceKey, osg::Shader::Type type);

		osg::ref_ptr<osgText::Font> loadDefaultFont();

		void setDefaultFontResourceKey(std::string resourceKey);
		void setResourceLoader(osg::ref_ptr<ResourceLoader> loader);

		void clearCacheResource(std::string resourceKey);
		void clearCache();

	private:
		typedef std::map<std::string, osg::ref_ptr<osg::Object>> ResourceDictionary;

		osg::ref_ptr<ResourceLoader> resourceLoader();

		char* loadBytesFromStream(std::ifstream& stream, long long length);
		std::string loadTextFromStream(std::ifstream& stream, long long length);
		osg::ref_ptr<osg::Object> loadObject(std::string resourceKey, ResourceType type = DETECT, osg::Shader::Type shaderType = osg::Shader::FRAGMENT);

		osg::ref_ptr<osg::Object> getCacheItem(std::string key);
		void storeCacheItem(std::string key, osg::ref_ptr<osg::Object> obj);

		ResourceDictionary _cache;
		std::string _defaultFontResourceKey;

		osg::ref_ptr<ResourceLoader> _resourceLoader;
	};
}