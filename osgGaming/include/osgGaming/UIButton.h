#pragma once

#include <osgGaming/UIVisualElement.h>
#include <osgGaming/UserInteractionModel.h>

#include <osg/Material>
#include <osgText/Text>

namespace osgGaming
{
	class UIButton : public UIVisualElement, public UserInteractionModel
	{
	public:
		UIButton();

		void setFontSize(float fontSize);
		void setText(std::string text);

		virtual void onMouseEnter() override;
		virtual void onMouseLeave() override;

		virtual void getAbsoluteOriginSize(osg::Vec2f& origin, osg::Vec2f& size) override;

		virtual void updatedContentOriginSize(osg::Vec2f origin, osg::Vec2f size) override;

	protected:
		virtual osg::Vec2f calculateMinContentSize() override;

	private:
		osg::ref_ptr<osg::Material> _material;

		std::string _text;
		int _fontSize;

		osg::ref_ptr<osgText::Text> _textNode;
	};
}