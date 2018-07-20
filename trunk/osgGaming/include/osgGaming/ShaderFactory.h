#pragma once

#include "osgGaming/AbstractFactory.h"

#include <map>

#include <osg/Shader>

namespace osgGaming
{
  class Injector;

  class ShaderBlueprint : public osg::Referenced
  {
  public:
    ShaderBlueprint();
    ~ShaderBlueprint();

    osg::ref_ptr<ShaderBlueprint> version(int shaderVersion);
    osg::ref_ptr<ShaderBlueprint> type(osg::Shader::Type shaderType);
    osg::ref_ptr<ShaderBlueprint> extension(const std::string& extName);
    osg::ref_ptr<ShaderBlueprint> module(const std::string& modulePath);

    osg::ref_ptr<osg::Shader> build();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };

  class ShaderFactory : public AbstractFactory<ShaderBlueprint>
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