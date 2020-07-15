#pragma once

#include <map>
//#include <locale>
//#include <codecvt>

#include <osgGaming/FileResourceLoader.h>

#include <osg/Image>
#include <osgText/Font>
#include <osg/Shader>

namespace osgGaming
{
  class Injector;

  class ResourceManager : public osg::Referenced
	{
	public:
		typedef enum _resourceType
		{
			DETECT,
			TEXT,
			BINARY,
			SHADER
		} ResourceType;

    explicit ResourceManager(Injector& injector);
    ~ResourceManager();

		std::string loadText(std::string resourceKey);
		char* loadBinary(std::string resourceKey);
		osg::ref_ptr<osg::Image> loadImage(std::string resourceKey);
		osg::ref_ptr<osgText::Font> loadFont(std::string resourceKey);
		osg::ref_ptr<osg::Shader> loadShader(std::string resourceKey, osg::Shader::Type type);

		void setResourceLoader(osg::ref_ptr<ResourceLoader> loader);

		void clearCacheResource(std::string resourceKey);
		void clearCache();

		static void setDefaultFont(const osg::ref_ptr<osgText::Font>& font);
		static osg::ref_ptr<osgText::Font> getDefaultFont();

	private:
		typedef std::map<std::string, osg::ref_ptr<osg::Object>> ResourceDictionary;

		osg::ref_ptr<ResourceLoader> resourceLoader();

		char* loadBytesFromStream(std::ifstream& stream, long long length);
		std::string loadTextFromStream(std::ifstream& stream, long long length);
		osg::ref_ptr<osg::Object> loadObject(std::string resourceKey, ResourceType type = DETECT, osg::Shader::Type shaderType = osg::Shader::FRAGMENT);

		osg::ref_ptr<osg::Object> getCacheItem(std::string key);
		void storeCacheItem(std::string key, osg::ref_ptr<osg::Object> obj);

		ResourceDictionary _cache;

		osg::ref_ptr<ResourceLoader> _resourceLoader;

	  static osg::ref_ptr<osgText::Font> m_defaultFont;

		// std::locale _utf8Locale;
	};
}