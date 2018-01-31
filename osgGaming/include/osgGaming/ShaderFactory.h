#pragma once

#include <map>

#include <osg/Shader>

namespace osgGaming
{
  class Injector;

  class ShaderFactory : public osg::Referenced
	{
	public:
    explicit ShaderFactory(Injector& injector);
    ~ShaderFactory();

		osg::ref_ptr<osg::Shader> fromSourceText(std::string key, std::string source, osg::Shader::Type type);

	private:
		typedef std::map<std::string, osg::ref_ptr<osg::Shader>> ShaderDictionary;

		ShaderDictionary m_shaderCache;
	};
}