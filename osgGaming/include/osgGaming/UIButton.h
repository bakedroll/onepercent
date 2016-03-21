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
    virtual void onClicked() override;
    virtual void onEnabledChanged(bool enabled) override;

		virtual void getAbsoluteOriginSize(osg::Vec2f& origin, osg::Vec2f& size) override;

		virtual void updatedContentOriginSize(osg::Vec2f origin, osg::Vec2f size) override;

    bool isCheckable();
    bool isChecked();

    void setCheckable(bool checkable);
    void setChecked(bool checked);

	protected:
		virtual osg::Vec2f calculateMinContentSize() override;

	private:
		osg::ref_ptr<osg::Material> m_material;

		std::string m_text;
		int m_fontSize;

		osg::ref_ptr<osgText::Text> m_textNode;

    bool m_checkable;
    bool m_isChecked;
	};
}