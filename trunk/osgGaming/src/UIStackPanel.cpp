#include <osgGaming/UIStackPanel.h>

using namespace osgGaming;
using namespace osg;

UIStackPanel::UIStackPanel()
	: UIElement(),
	  _spacing(5.0f)
{

}

void UIStackPanel::getOriginSizeForChildInArea(osg::ref_ptr<UIElement> child, Vec2f area, Vec2f& origin, Vec2f& size)
{
	int i = 0;

	float num = (float)getNumUIChildren();

	switch (_orientation)
	{
	case HORIZONTAL:

		size.x() = (area.x() - ((num - 1.0f) * _spacing)) / num;
		size.y() = area.y();

		origin.x() = i * (size.x() + _spacing);
		origin.y() = 0.0f;

		break;

	case VERTICAL:

		size.x() = area.x();
		size.y() = (area.y() - ((num - 1.0f) * _spacing)) / num;

		origin.x() = 0.0f;
		origin.y() = i * (size.y() + _spacing);

		break;

	}
}

void UIStackPanel::setSpacing(float spacing)
{
	_spacing = spacing;
}

void UIStackPanel::setOrientation(Orientation orientation)
{
	_orientation = orientation;
}