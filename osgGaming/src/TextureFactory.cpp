#include <osgGaming/TextureFactory.h>

namespace osgGaming
{
  struct TextureBlueprint::Impl
  {
    Impl() 
      : texLayer(0)
      , dataVariance(osg::Object::DYNAMIC)
      , wrapS(osg::Texture::CLAMP_TO_EDGE)
      , wrapT(osg::Texture::CLAMP_TO_EDGE)
      , minFilter(osg::Texture::LINEAR_MIPMAP_LINEAR)
      , magFilter(osg::Texture::LINEAR)
      , maxAnisotropy(8.0f)
    {}

    typedef struct _bpUniform
    {
      std::string uniformName;
      osg::ref_ptr<osg::StateSet> stateSet;
    } BpUniform;

    typedef std::vector<BpUniform> BpUniformList;
    typedef std::vector<osg::ref_ptr<osg::StateSet>> StateSetList;

    osg::ref_ptr<osg::Image> image;

    int texLayer;

    osg::Object::DataVariance dataVariance;
    osg::Texture::WrapMode wrapS;
    osg::Texture::WrapMode wrapT;
    osg::Texture::FilterMode minFilter;
    osg::Texture::FilterMode magFilter;

    float maxAnisotropy;

    BpUniformList bpUniforms;
    StateSetList assignToStateSets;
  };

  TextureBlueprint::TextureBlueprint()
    : Referenced()
    , m(new Impl())
  {

  }

  TextureBlueprint::~TextureBlueprint()
  {
  }

  osg::ref_ptr<TextureBlueprint> TextureBlueprint::image(osg::ref_ptr<osg::Image> img)
  {
    m->image = img;

    return this;
  }

  osg::ref_ptr<TextureBlueprint> TextureBlueprint::texLayer(int texLayer)
  {
    m->texLayer = texLayer;

    return this;
  }

  osg::ref_ptr<TextureBlueprint> TextureBlueprint::assign(osg::ref_ptr<osg::StateSet> stateSet)
  {
    m->assignToStateSets.push_back(stateSet);

    return this;
  }

  osg::ref_ptr<TextureBlueprint> TextureBlueprint::uniform(osg::ref_ptr<osg::StateSet> stateSet, std::string uniformName)
  {
    Impl::BpUniform uniform;
    uniform.stateSet = stateSet;
    uniform.uniformName = uniformName;

    m->bpUniforms.push_back(uniform);

    return this;
  }

  osg::ref_ptr<osg::Texture2D> TextureBlueprint::build()
  {
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
    texture->setDataVariance(m->dataVariance);
    texture->setWrap(osg::Texture::WRAP_S, m->wrapS);
    texture->setWrap(osg::Texture::WRAP_T, m->wrapT);
    texture->setFilter(osg::Texture::MIN_FILTER, m->minFilter);
    texture->setFilter(osg::Texture::MAG_FILTER, m->magFilter);
    texture->setMaxAnisotropy(m->maxAnisotropy);

    if (m->image.valid())
    {
      texture->setImage(m->image);
    }

    for (Impl::StateSetList::iterator it = m->assignToStateSets.begin(); it != m->assignToStateSets.end(); ++it)
    {
      it->get()->setTextureAttributeAndModes(m->texLayer, texture, osg::StateAttribute::ON);
    }

    for (Impl::BpUniformList::iterator it = m->bpUniforms.begin(); it != m->bpUniforms.end(); ++it)
    {
      osg::ref_ptr<osg::Uniform> uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D, it->uniformName);
      uniform->set(m->texLayer);
      it->stateSet->addUniform(uniform);
    }

    return texture;
  }
}