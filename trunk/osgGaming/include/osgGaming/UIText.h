#pragma once

#include <osgGaming/UIVisualElement.h>

#include <osgText/Text>

namespace osgGaming
{
	class UIText : public UIVisualElement
	{
	public:
    typedef osg::ref_ptr<UIText> Ptr;

		UIText();

		void setText(std::string text);
		void setFontSize(int size);

		void setTextAlignment(osgText::TextBase::AlignmentType alignment);
    void setOutlineColor(const osg::Vec4f& color);
    void setOutlineEnabled(bool enabled);

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