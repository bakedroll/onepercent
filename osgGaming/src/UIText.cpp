#include <osgGaming/UIText.h>

#include <osg/Geode>

using namespace osgGaming;
using namespace osgText;
using namespace osg;

UIText::UIText()
	: UIElement(),
	  _text(""),
	  _fontSize(25),
	  _horizontalAlignment(LEFT),
	  _verticalAlignment(TOP)
{
	ref_ptr<Geode> geode = new Geode();

	_textNode = new Text();
	_textNode->setCharacterSize(_fontSize);
	_textNode->setFont("./data/fonts/coolvetica rg.ttf");
	_textNode->setText(_text);
	_textNode->setAxisAlignment(osgText::Text::SCREEN);
	_textNode->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	_textNode->setDataVariance(osg::Object::DYNAMIC);

	addChild(geode);
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

void UIText::setHorizontalAlignment(HorizontalAlignment alignment)
{
	_horizontalAlignment = alignment;
}

void UIText::setVerticalAlignment(VerticalAlignment alignment)
{
	_verticalAlignment = alignment;
}

void UIText::updatedContentOriginSize(Vec2f origin, Vec2f size)
{
	Vec2f position;

	switch (_horizontalAlignment)
	{
	case LEFT:
		position.x() = origin.x();
		break;
	case RIGHT:
		position.x() = origin.x() + size.x();
		break;
	case CENTER:
		position.x() = origin.x() + size.x() / 2.0f;
		break;
	}

	switch (_verticalAlignment)
	{
	case TOP:
		position.y() = origin.y() + size.y();
		break;
	case BOTTOM:
		position.y() = origin.y();
		break;
	case MIDDLE:
		position.y() = origin.y() + size.y() / 2.0f;
		break;
	}

	if (_verticalAlignment == TOP)
	{
		if (_horizontalAlignment == LEFT)
		{
			_textNode->setAlignment(TextBase::LEFT_TOP);
		}
		else if (_horizontalAlignment == CENTER)
		{
			_textNode->setAlignment(TextBase::CENTER_TOP);
		}
		else if (_horizontalAlignment == RIGHT)
		{
			_textNode->setAlignment(TextBase::RIGHT_TOP);
		}
	}
	else if (_verticalAlignment == MIDDLE)
	{
		if (_horizontalAlignment == LEFT)
		{
			_textNode->setAlignment(TextBase::LEFT_CENTER);
		}
		else if (_horizontalAlignment == CENTER)
		{
			_textNode->setAlignment(TextBase::CENTER_CENTER);
		}
		else if (_horizontalAlignment == RIGHT)
		{
			_textNode->setAlignment(TextBase::RIGHT_CENTER);
		}
	}
	else if (_verticalAlignment == BOTTOM)
	{
		if (_horizontalAlignment == LEFT)
		{
			_textNode->setAlignment(TextBase::LEFT_BOTTOM);
		}
		else if (_horizontalAlignment == CENTER)
		{
			_textNode->setAlignment(TextBase::CENTER_BOTTOM);
		}
		else if (_horizontalAlignment == RIGHT)
		{
			_textNode->setAlignment(TextBase::RIGHT_BOTTOM);
		}
	}

	_textNode->setPosition(osg::Vec3(position.x(), position.y(), 0.0f));
}