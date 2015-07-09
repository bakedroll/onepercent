#pragma once

#include <string>

#include <osg/Object>
#include <osg/Referenced>
#include <osg/Texture2D>
#include <osg/Image>

#include <osgGaming/Singleton.h>

namespace osgGaming
{
	class TextureBlueprint : public osg::Referenced
	{
	public:
		TextureBlueprint();

		osg::ref_ptr<TextureBlueprint> image(osg::ref_ptr<osg::Image> img);
		osg::ref_ptr<TextureBlueprint> texLayer(int texLayer);
		osg::ref_ptr<TextureBlueprint> assign(osg::ref_ptr<osg::StateSet> stateSet);
		osg::ref_ptr<TextureBlueprint> uniform(osg::ref_ptr<osg::StateSet> stateSet, std::string uniformName);

		osg::ref_ptr<osg::Texture2D> build();

	private:
		typedef struct _bpUniform
		{
			std::string uniformName;
			osg::ref_ptr<osg::StateSet> stateSet;
		} BpUniform;

		typedef std::vector<BpUniform> BpUniformList;
		typedef std::vector<osg::ref_ptr<osg::StateSet>> StateSetList;

		osg::ref_ptr<osg::Image> _image;

		int _texLayer;

		osg::Object::DataVariance _dataVariance;
		osg::Texture::WrapMode _wrapS;
		osg::Texture::WrapMode _wrapT;
		osg::Texture::FilterMode _minFilter;
		osg::Texture::FilterMode _magFilter;

		float _maxAnisotropy;

		BpUniformList _bpUniforms;
		StateSetList _assignToStateSets;
	};

	class TextureFactory : public Singleton<TextureFactory>
	{
	public:
		osg::ref_ptr<TextureBlueprint> make();
	};
}