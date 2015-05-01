#include <osgGaming/UIText.h>

#include <osg/Geode>

using namespace osgGaming;
using namespace osgText;
using namespace osg;

UIText::UIText()
	: UIVisualElement(),
	  _text(""),
	  _fontSize(25),
	  _alignment(TextBase::LEFT_TOP)
{
	ref_ptr<Geode> geode = new Geode();

	_textNode = new Text();
	_textNode->setCharacterSize(_fontSize);
	_textNode->setFont("./data/fonts/coolvetica rg.ttf");
	_textNode->setText(_text);
	_textNode->setAxisAlignment(osgText::Text::SCREEN);
	_textNode->setDrawMode(osgText::Text::TEXT /*| osgText::Text::BOUNDINGBOX*/);
	_textNode->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	_textNode->setDataVariance(osg::Object::DYNAMIC);

	MatrixTransform::addChild(geode);
	geode->addDrawable(_textNode);
}

void UIText::setText(std::string text)
{
	_text = text;
	_textNode->setText(text);
}

void UIText::setFontSize(int size)
{
	_fontSize = size;
	_textNode->setCharacterSize(size);
}

void UIText::setTextAlignment(TextBase::AlignmentType alignment)
{
	_alignment = alignment;
}

void UIText::updatedContentOriginSize(Vec2f origin, Vec2f size)
{
	Vec2f position;

	if (_alignment == TextBase::LEFT_BOTTOM || _alignment == TextBase::LEFT_CENTER || _alignment == TextBase::LEFT_TOP)
	{
		position.x() = origin.x();
	}
	else if (_alignment == TextBase::RIGHT_BOTTOM || _alignment == TextBase::RIGHT_CENTER || _alignment == TextBase::RIGHT_TOP)
	{
		position.x() = origin.x() + size.x();
	}
	else if (_alignment == TextBase::CENTER_BOTTOM || _alignment == TextBase::CENTER_CENTER || _alignment == TextBase::CENTER_TOP)
	{
		position.x() = origin.x() + size.x() / 2.0f;
	}

	if (_alignment == TextBase::LEFT_TOP || _alignment == TextBase::CENTER_TOP || _alignment == TextBase::RIGHT_TOP)
	{
		position.y() = origin.y() + size.y();
	}
	else if (_alignment == TextBase::LEFT_BOTTOM || _alignment == TextBase::CENTER_BOTTOM || _alignment == TextBase::RIGHT_BOTTOM)
	{
		position.y() = origin.y();
	}
	else if (_alignment == TextBase::LEFT_CENTER || _alignment == TextBase::CENTER_CENTER || _alignment == TextBase::RIGHT_CENTER)
	{
		position.y() = origin.y() + size.y() / 2.0f;
	}

	_textNode->setAlignment(_alignment);
	_textNode->setPosition(Vec3(position.x(), position.y(), 0.0f));
}

Vec2f UIText::calculateMinContentSize()
{
	//_textNode->getTextureGlyphQuadMap();

	BoundingBox bb = _textNode->getBound();

	//float cheight =_textNode->getCharacterHeight();

	float width = bb.xMax() - bb.xMin();
	float height = bb.yMax() - bb.yMin();

	return Vec2f(width, height);
}