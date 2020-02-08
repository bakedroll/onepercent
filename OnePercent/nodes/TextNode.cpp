#include "nodes/TextNode.h"

#include <osg/Billboard>

namespace onep
{

void TextNode::Definition::registerDefinition(lua_State* state)
{
  getGlobalNamespace(state)
          .deriveClass<TextNode, osg::Node>("TextNode")
          .addFunction("set_text", &TextNode::luaSetText)
          .endClass();
}

TextNode::TextNode()
  : TextNode("")
{

}

TextNode::TextNode(const std::string& text)
  : osg::MatrixTransform()
{
  osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard();
  billboard->setMode(osg::Billboard::Mode::POINT_ROT_EYE);

  m_textNode = new osgText::Text();

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

  setMatrix(matrix);
}

TextNode::~TextNode() = default;

void TextNode::luaSetText(const std::string& text)
{
  m_textNode->setText(text);
}

}
