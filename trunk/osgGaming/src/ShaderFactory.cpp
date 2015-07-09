#include <osgGaming/ShaderFactory.h>

using namespace std;
using namespace osg;
using namespace osgGaming;

ref_ptr<ShaderFactory> Singleton<ShaderFactory>::_instance;

ref_ptr<Shader> ShaderFactory::fromSourceText(string key, string source, Shader::Type type)
{
	ShaderDictionary::iterator it = _shaderCache.find(key);
	
	if (it != _shaderCache.end())
	{
		return it->second;
	}

	ref_ptr<Shader> shader = new Shader(type);
	shader->setShaderSource(source);

	_shaderCache.insert(ShaderDictionary::value_type(key, shader));

	return shader;
}