#pragma once

#include "osgGaming/AbstractFactory.h"

#include <string>
#include <memory>

#include <osg/Object>
#include <osg/Referenced>
#include <osg/Texture2D>
#include <osg/Image>

namespace osgGaming
{
  class Injector;

  class TextureBlueprint : public osg::Referenced
	{
	public:
		TextureBlueprint();
    ~TextureBlueprint();
		osg::ref_ptr<TextureBlueprint> image(osg::ref_ptr<osg::Image> img);

		osg::ref_ptr<TextureBlueprint> texLayer(int texLayer);
		osg::ref_ptr<TextureBlueprint> assign(osg::ref_ptr<osg::StateSet> stateSet);
		osg::ref_ptr<TextureBlueprint> uniform(osg::ref_ptr<osg::StateSet> stateSet, std::string uniformName);

		osg::ref_ptr<osg::Texture2D> build();

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};

	class TextureFactory : public AbstractFactory<TextureBlueprint>
  {
  public:
	  explicit TextureFactory(Injector& injector)
	    : AbstractFactory<TextureBlueprint>(injector)
	  {
	  }
	};
}