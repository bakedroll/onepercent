#pragma once

#include <map>

#include <osg/Referenced>
#include <osg/Image>
#include <osgText/Font>
#include <osgDB/ReadFile>
#include <osg/Shader>

namespace osgGaming
{
	class ResourceManager : public osg::Referenced
	{
	public:
		typedef std::map<std::string, osg::ref_ptr<osg::Object>> ResourceDictionary;

		static osg::ref_ptr<ResourceManager> getInstance();

		osg::ref_ptr<osg::Image> loadImage(std::string resourceKey);
		osg::ref_ptr<osgText::Font> loadFont(std::string resourceKey);
		osg::ref_ptr<osg::Shader> loadShader(std::string resourceKey, osg::Shader::Type type);

		osg::ref_ptr<osgText::Font> loadDefaultFont();

		void setDefaultFontResourceKey(std::string resourceKey);

	protected:
		ResourceManager();

	private:
		static osg::ref_ptr<ResourceManager> _instance;

		osg::ref_ptr<osg::Object> loadObject(std::string resourceKey);

		osg::ref_ptr<osg::Object> getCacheItem(std::string key);
		void storeCacheItem(std::string key, osg::ref_ptr<osg::Object> obj);

		ResourceDictionary _cache;
		std::string _defaultFontResourceKey;
	};
}