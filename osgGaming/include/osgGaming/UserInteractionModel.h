#pragma once

#include <osg/Vec2>

namespace osgGaming
{
	class UserInteractionModel
	{
	public:
		UserInteractionModel();

		virtual void onMouseEnter() = 0;
		virtual void onMouseLeave() = 0;
		
		virtual void getAbsoluteOriginSize(osg::Vec2f& origin, osg::Vec2f& size) = 0;

		bool getHovered();
		void setHovered(bool hovered);

	private:
		bool _hovered;
	};
}