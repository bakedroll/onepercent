#pragma once

#include <vector>

#include <osg/Vec2>

namespace osgGaming
{
	class UserInteractionModel
	{
	public:
    typedef std::vector<UserInteractionModel*> List;

    virtual ~UserInteractionModel();

	  UserInteractionModel();

		virtual void onMouseEnter() = 0;
		virtual void onMouseLeave() = 0;
    virtual void onClicked();
    virtual void onEnabledChanged(bool enabled);
		
		virtual void getAbsoluteOriginSize(osg::Vec2f& origin, osg::Vec2f& size) = 0;

    bool isEnabled();
		bool getHovered();

	  virtual void setEnabled(bool enabled);
		void setHovered(bool hovered);

	private:
		bool m_hovered;
    bool m_enabled;

	};
}