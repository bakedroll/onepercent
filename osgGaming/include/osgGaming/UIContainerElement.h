#pragma once

#include <osgGaming/UIContainerElementBase.h>
#include <osgGaming/GameException.h>

#include <osg/MatrixTransform>

#include <map>

namespace osgGaming
{
	template<typename UILocationType>
	class UIContainerElement : public UIContainerElementBase
	{
	public:
		typedef std::map<osg::ref_ptr<UIElement>, UILocationType> ChildLocationMap;

		virtual bool addChild(osg::Node* node) override
		{
			return addChild(node, UILocationType());
		}

		virtual bool addChild(osg::Node* node, UILocationType location)
		{
			osg::ref_ptr<UIElement> uiElement = dynamic_cast<UIElement*>(node);
			if (uiElement.valid())
			{
				ChildLocationMap::iterator it = _childLocationMap.find(uiElement);

				if (it != _childLocationMap.end())
				{
					throw GameException("Inconsistent state at UIContainerElement::addChild()\n");
				}

				_childLocationMap.insert(ChildLocationMap::value_type(uiElement, location));
			}

			return osg::MatrixTransform::addChild(node);
		}

		virtual void getOriginSizeForChildInArea(osg::ref_ptr<UIElement> child, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size) override
		{
			UILocationType location = _childLocationMap.find(child)->second;

			getOriginSizeForLocationInArea(location, area, origin, size);
		}

		virtual void getOriginSizeForLocationInArea(UILocationType location, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size) = 0;

	protected:
		UILocationType getLocationOfChild(osg::ref_ptr<UIElement> child)
		{
			return _childLocationMap.find(child)->second;
		}

	private:
		ChildLocationMap _childLocationMap;

	};
}