#pragma once

#include <osgGaming/UIVisualElement.h>
#include <osgGaming/UserInteractionModel.h>

#include <osg/Material>

namespace osgGaming
{
	class UIButton : public UIVisualElement, public UserInteractionModel
	{
	public:
		UIButton();

		virtual void onMouseEnter() override;
		virtual void onMouseLeave() override;

		virtual void getAbsoluteOriginSize(osg::Vec2f& origin, osg::Vec2f& size) override;

	private:
		osg::ref_ptr<osg::Material> _material;
	};
}