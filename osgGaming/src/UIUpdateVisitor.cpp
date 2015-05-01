#include <osgGaming/UIUpdateVisitor.h>
#include <osgGaming/UIVisualElement.h>
#include <osgGaming/UIContainerElement.h>

#include <math.h>

using namespace osg;
using namespace osgGaming;
using namespace std;

UIUpdateVisitor::UIUpdateVisitor()
	: NodeVisitor(TRAVERSE_ALL_CHILDREN),
	  _traversedFirst(false)
{

}

void UIUpdateVisitor::apply(osg::Node &node)
{
	ref_ptr<UIElement> uiElement = dynamic_cast<UIElement*>(&node);

	if (uiElement.valid())
	{
		if (_traversedFirst == false)
		{
			uiElement->resetMinContentSize();
			_traversedFirst = true;
		}

		Vec2f size = uiElement->getSize();
		Vec2f contentSize = uiElement->getContentSize();
		Vec4f margin = uiElement->getMargin();
		Vec4f padding = uiElement->getPadding();

		Vec2f shift(margin.x() + padding.x(), margin.w() + padding.w());

		Vec2f minContentSize = uiElement->getMinContentSize();
		if (minContentSize.x() > contentSize.x() || minContentSize.y() > contentSize.y())
		{
			Vec2f add(fmaxf(0.0f, minContentSize.x() - contentSize.x()), fmaxf(0.0f, minContentSize.y() - contentSize.y()));

			contentSize += add;
			size += add;

			uiElement->setSize(size);
		}

		ref_ptr<UIVisualElement> uiVisualElement = dynamic_cast<UIVisualElement*>(&node);
		if (uiVisualElement.valid())
		{
			uiVisualElement->updatedContentOriginSize(shift, contentSize);
		}

		ref_ptr<UIContainerElementBase> uiContainerElement = dynamic_cast<UIContainerElementBase*>(&node);
		if (uiContainerElement.valid())
		{
			UIContainerElementBase::UIElementList children = uiContainerElement->getUIChildren();

			for (UIContainerElementBase::UIElementList::iterator it = children.begin(); it != children.end(); ++it)
			{
				Vec2f childFrameOrigin, childFrameSize, childOrigin, childSize;

				uiContainerElement->getOriginSizeForChildInArea(*it, contentSize, childFrameOrigin, childFrameSize);

				getOriginSizeInArea(*it, childFrameSize, childOrigin, childSize);

				childFrameOrigin.x() += (childOrigin.x() + margin.x() + padding.x());
				childFrameOrigin.y() += (childOrigin.y() + margin.w() + padding.w());

				(*it)->setAbsoluteOrigin(uiElement->getAbsoluteOrigin() + childFrameOrigin);
				(*it)->setOrigin(childFrameOrigin);
				(*it)->setSize(childSize);
			}
		}
	}

	traverse(node);
}

void UIUpdateVisitor::getOriginSizeInArea(ref_ptr<UIElement> element, Vec2f area, Vec2f& origin, Vec2f& size)
{
	UIElement::HorizontalAlignment halign = element->getHorizontalAlignment();
	UIElement::VerticalAlignment valign = element->getVerticalAlignment();

	float width = element->getWidth();
	float height = element->getHeight();

	width = width > area.x() ? area.x() : width;
	height = height > area.y() ? area.y() : height;

	switch (halign)
	{
	case UIElement::LEFT:
		size.x() = width;
		origin.x() = 0.0f;
		break;

	case UIElement::RIGHT:
		size.x() = width;
		origin.x() = area.x() - width;
		break;

	case UIElement::CENTER:
		size.x() = width;
		origin.x() = (area.x() / 2.0f) - (width / 2.0f);
		break;

	case UIElement::H_STRETCH:
		size.x() = area.x();
		origin.x() = 0.0f;
		break;

	}

	switch (valign)
	{
	case UIElement::TOP:
		size.y() = height;
		origin.y() = area.y() - height;
		break;

	case UIElement::BOTTOM:
		size.y() = height;
		origin.y() = 0.0f;
		break;

	case UIElement::MIDDLE:
		size.y() = height;
		origin.y() = (area.y() / 2.0f) - (height / 2.0f);
		break;

	case UIElement::V_STRETCH:
		size.y() = area.y();
		origin.y() = 0.0f;
		break;

	}
}
