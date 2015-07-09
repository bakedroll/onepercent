#pragma once

#include <string>
#include <map>

#include <osgGaming/Singleton.h>

#include <osg/Shader>

namespace osgGaming
{
	class ShaderFactory : public Singleton<ShaderFactory>
	{
	public:
		osg::ref_ptr<osg::Shader> fromSourceText(std::string key, std::string source, osg::Shader::Type type);

	private:
		typedef std::map<std::string, osg::ref_ptr<osg::Shader>> ShaderDictionary;

		ShaderDictionary _shaderCache;
	};
}