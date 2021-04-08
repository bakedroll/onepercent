#include "nodes/TextNode.h"

#include <osg/Billboard>

#include <osgDB/ReadFile>

#include <osgHelper/ResourceManager.h>

namespace onep
{

void TextNode::Definition::registerDefinition(lua_State* state)
{
  getGlobalNamespace(state)
          .deriveClass<TextNode, LuaVisualOsgNode<osg::MatrixTransform>>("TextNode")
          .addFunction("set_text", &TextNode::luaSetText)
          .endClass();
}

TextNode::TextNode()
  : TextNode("")
{

}

TextNode::TextNode(const std::string& text)
  : LuaVisualOsgNode<osg::MatrixTransform>()
{
  getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard();
  billboard->setMode(osg::Billboard::Mode::POINT_ROT_EYE);

  m_textNode = new osgText::Text();

  auto font = osgHelper::ResourceManager::getDefaultFont();
  if (font)
  {
    m_textNode->setFont(font);
  }

  m_textNode->setCharacterSize(10);
  m_textNode->setText(text);
  m_textNode->setAlignment(osgText::Text::AlignmentType::CENTER_CENTER);
  m_textNode->setAxisAlignment(osgText::Text::AxisAlignment::XZ_PLANE);
  m_textNode->setDrawMode(osgText::Text::TEXT);
  m_textNode->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

  billboard->addDrawable(m_textNode);

  addChild(billboard);

  auto matrix = osg::Matrix::identity();
  matrix *= osg::Matrix::scale(osg::Vec3f(0.01f, 0.01f, 0.01f));
  matrix *= osg::Matrix::translate(osg::Vec3f(0.0f, -0.3f, 0.0f));

  setMatrix(matrix);
}

TextNode::~TextNode() = default;

void TextNode::luaSetText(const std::string& text)
{
  m_textNode->setText(text);
}

}
