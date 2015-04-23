#include <osgGaming/UIUpdateVisitor.h>

using namespace osg;
using namespace osgGaming;

UIUpdateVisitor::UIUpdateVisitor()
	: NodeVisitor(TRAVERSE_ALL_CHILDREN)
{

}

void UIUpdateVisitor::apply(osg::Node &node)
{
	ref_ptr<UIElement> uiElement = dynamic_cast<UIElement*>(&node);

	if (uiElement.valid())
	{
		UIElement::UIElementList children = uiElement->getUIChildren();

		Vec2f contentSize = uiElement->getSize();
		Vec2f contentOrigin = uiElement->getOrigin();
		Vec4f margin = uiElement->getMargin();
		Vec4f padding = uiElement->getPadding();

		contentSize.x() -= (margin.x() + margin.z() + padding.x() + padding.z());
		contentSize.y() -= (margin.y() + margin.w() + padding.y() + padding.w());

		Vec2f shift(margin.x() + padding.x(), margin.w() + padding.w());

		contentOrigin += shift;

		uiElement->updatedContentOriginSize(shift, contentSize);

		for (unsigned int i = 0; i < children.size(); i++)
		{
			ref_ptr<UIElement> child = children.at(i).get();

			Vec2f childFrameOrigin, childFrameSize, childOrigin, childSize;

			uiElement->getOriginSizeForChildInArea(i, contentSize, childFrameOrigin, childFrameSize);

			getOriginSizeInArea(child, childFrameSize, childOrigin, childSize);

			childFrameOrigin.x() += (childOrigin.x() + margin.x() + padding.x());
			childFrameOrigin.y() += (childOrigin.y() + margin.w() + padding.w());

			child->setOrigin(childFrameOrigin);
			child->setSize(childSize);
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
