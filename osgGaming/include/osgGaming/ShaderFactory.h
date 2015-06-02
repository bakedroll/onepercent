#pragma once

#include <string>
#include <map>

#include <osg/Referenced>
#include <osg/Shader>

namespace osgGaming
{
	class ShaderFactory : public osg::Referenced
	{
	public:
		static osg::ref_ptr<osg::Shader> fromSourceText(std::string key, std::string source, osg::Shader::Type type);

	private:
		typedef std::map<std::string, osg::ref_ptr<osg::Shader>> ShaderDictionary;

		static ShaderDictionary _shaderCache;
	};
}