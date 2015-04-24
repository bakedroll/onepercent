#pragma once

#include <osgGaming/UIElement.h>

#include <osgText/Text>

#include <string.h>

namespace osgGaming
{
	class UIText : public UIElement
	{
	public:
		UIText();

		void setText(std::string text);
		void setFontSize(int size);

		void setTextAlignment(osgText::TextBase::AlignmentType alignment);

		virtual void updatedContentOriginSize(osg::Vec2f origin, osg::Vec2f size) override;

	protected:
		virtual osg::Vec2f calculateMinContentSize() override;

	private:
		std::string _text;
		int _fontSize;

		osg::ref_ptr<osgText::Text> _textNode;

		osgText::TextBase::AlignmentType _alignment;
	};
}