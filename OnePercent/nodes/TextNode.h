#pragma once

#include "scripting/LuaBridgeDefinition.h"

#include <osg/MatrixTransform>
#include <osgText/Text>

namespace onep
{
  class TextNode : public osg::MatrixTransform
  {
  public:
    class Definition : public LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    TextNode();
    explicit TextNode(const std::string& text);
    ~TextNode();

    void luaSetText(const std::string& text);

  private:
      osg::ref_ptr<osgText::Text> m_textNode;
    
  };
}  // namespace onep