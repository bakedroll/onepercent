#include <osgGaming/TextureFactory.h>

using namespace osgGaming;
using namespace osg;
using namespace std;

TextureBlueprint::TextureBlueprint()
	: Referenced(),
	  _texLayer(0),
	  _dataVariance(Object::DYNAMIC),
	  _wrapS(Texture::CLAMP_TO_EDGE),
	  _wrapT(Texture::CLAMP_TO_EDGE),
	  _minFilter(Texture::LINEAR_MIPMAP_LINEAR),
	  _magFilter(Texture::LINEAR),
	  _maxAnisotropy(8.0f)
{

}

ref_ptr<TextureBlueprint> TextureBlueprint::image(ref_ptr<Image> img)
{
	_image = img;

	return this;
}

ref_ptr<TextureBlueprint> TextureBlueprint::texLayer(int texLayer)
{
	_texLayer = texLayer;

	return this;
}

ref_ptr<TextureBlueprint> TextureBlueprint::assign(ref_ptr<StateSet> stateSet)
{
	_assignToStateSets.push_back(stateSet);

	return this;
}

ref_ptr<TextureBlueprint> TextureBlueprint::uniform(ref_ptr<StateSet> stateSet, string uniformName)
{
	BpUniform uniform;
	uniform.stateSet = stateSet;
	uniform.uniformName = uniformName;

	_bpUniforms.push_back(uniform);

	return this;
}

ref_ptr<Texture2D> TextureBlueprint::build()
{
	ref_ptr<Texture2D> texture = new Texture2D();
	texture->setDataVariance(_dataVariance);
	texture->setWrap(Texture::WRAP_S, _wrapS);
	texture->setWrap(Texture::WRAP_T, _wrapT);
	texture->setFilter(Texture::MIN_FILTER, _minFilter);
	texture->setFilter(Texture::MAG_FILTER, _magFilter);
	texture->setMaxAnisotropy(_maxAnisotropy);

	if (_image.valid())
	{
		texture->setImage(_image);
	}

	for (StateSetList::iterator it = _assignToStateSets.begin(); it != _assignToStateSets.end(); ++it)
	{
		it->get()->setTextureAttributeAndModes(_texLayer, texture, StateAttribute::ON);
	}

	for (BpUniformList::iterator it = _bpUniforms.begin(); it != _bpUniforms.end(); ++it)
	{
		ref_ptr<Uniform> uniform = new Uniform(Uniform::SAMPLER_2D, it->uniformName);
		uniform->set(_texLayer);
		it->stateSet->addUniform(uniform);
	}

	return texture;
}

ref_ptr<TextureFactory> Singleton<TextureFactory>::_instance;

ref_ptr<TextureBlueprint> TextureFactory::make()
{
	return new TextureBlueprint();
}