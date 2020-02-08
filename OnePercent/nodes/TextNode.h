#pragma once

#include "scripting/LuaVisualOsgNode.h"
#include "scripting/LuaBridgeDefinition.h"

#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osgText/Font>

namespace onep
{
  class TextNode : public LuaVisualOsgNode<osg::MatrixTransform>
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

    static osg::ref_ptr<osgText::Font> getDefaultFont();
    static osg::ref_ptr<osgText::Font> s_font;
    
  };
}  // namespace onep