#pragma once

#include <string>

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
		std::string getUIMName();

		void setHovered(bool hovered);
		void setUIMName(std::string name);

	private:
		bool _hovered;

		std::string _name;
	};
}