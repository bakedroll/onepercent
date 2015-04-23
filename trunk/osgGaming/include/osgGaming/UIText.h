#pragma once

#include <osgGaming/UIElement.h>

#include <osgText/Text>

#include <string.h>

namespace osgGaming
{
	class UIText : public UIElement
	{
	public:
		typedef enum _horizontalAlignment
		{
			LEFT,
			CENTER,
			RIGHT
		} HorizontalAlignment;

		typedef enum _verticalAlignment
		{
			TOP,
			MIDDLE,
			BOTTOM
		} VerticalAlignment;

		UIText();

		void setText(std::string text);
		void setFontSize(int size);

		void setHorizontalAlignment(HorizontalAlignment alignment);
		void setVerticalAlignment(VerticalAlignment alignment);

		virtual void updatedContentOriginSize(osg::Vec2f origin, osg::Vec2f size) override;

	private:
		std::string _text;
		int _fontSize;

		osg::ref_ptr<osgText::Text> _textNode;

		HorizontalAlignment _horizontalAlignment;
		VerticalAlignment _verticalAlignment;
	};
}