#pragma once

#include "scripting/LuaVisualOsgNode.h"

#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osgText/Font>

#include <luaHelper/LuaBridgeDefinition.h>

namespace onep
{
  class TextNode : public LuaVisualOsgNode<osg::MatrixTransform>
  {
  public:
    class Definition : public luaHelper::LuaBridgeDefinition
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