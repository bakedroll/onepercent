#include <osgGaming/ShaderFactory.h>

namespace osgGaming
{

  ShaderFactory::ShaderFactory(Injector& injector)
  {
  }

  ShaderFactory::~ShaderFactory()
  {
  }

  osg::ref_ptr<osg::Shader> ShaderFactory::fromSourceText(std::string key, std::string source, osg::Shader::Type type)
  {
    ShaderDictionary::iterator it = m_shaderCache.find(key);

    if (it != m_shaderCache.end())
    {
      return it->second;
    }

    osg::ref_ptr<osg::Shader> shader = new osg::Shader(type);
    shader->setShaderSource(source);

    m_shaderCache.insert(ShaderDictionary::value_type(key, shader));

    return shader;
  }

}