#include <osgGaming/UIStackPanel.h>

using namespace osgGaming;
using namespace osg;

UIStackPanel::UIStackPanel()
	: UIContainerElement(),
	  _spacing(5.0f)
{
	_cells = new UICells();

	_cells->setNumCells(1);
}

ref_ptr<UICells> UIStackPanel::getCells()
{
	return _cells;
}

void UIStackPanel::getOriginSizeForLocationInArea(int location, Vec2f area, Vec2f& origin, Vec2f& size)
{
	switch (_orientation)
	{
	case HORIZONTAL:

		_cells->getActualOriginSize(location, area.x(), _spacing, origin.x(), size.x());

		size.y() = area.y();
		origin.y() = 0.0f;

		break;

	case VERTICAL:

		_cells->getActualOriginSize(location, area.y(), _spacing, origin.y(), size.y());

		size.x() = area.x();
		origin.x() = 0.0f;

		break;

	}
}

UIStackPanel::Orientation UIStackPanel::getOrientation()
{
  return _orientation;
}

void UIStackPanel::setSpacing(float spacing)
{
	_spacing = spacing;
}

void UIStackPanel::setOrientation(Orientation orientation)
{
  _cells->setReverseMode(orientation == VERTICAL);
	_orientation = orientation;
}

Vec2f UIStackPanel::calculateMinContentSize()
{
	_cells->reset();

	UIElementList children = getUIChildren();

	float minLaneSize = 0.0f;
	for (UIElementList::iterator it = children.begin(); it != children.end(); ++it)
	{
		ref_ptr<UIElement> child = *it;

		int loc = getLocationOfChild(child);
		Vec2f minSize = child->getMinSize();

		_cells->expandCell(loc, _orientation == HORIZONTAL ? minSize.x() : minSize.y());
		minLaneSize = fmaxf(minLaneSize, _orientation == HORIZONTAL ? minSize.y() : minSize.x());
	}


	float minContentOrientationSize = (float(_cells->getNumCells() - 1) * _spacing);

	for (int i = 0; i < _cells->getNumCells(); i++)
	{
		float size;
		_cells->getMinSize(i, size);

		minContentOrientationSize += size;
	}

	return _orientation == HORIZONTAL ?
		Vec2f(minContentOrientationSize, minLaneSize) :
		Vec2f(minLaneSize, minContentOrientationSize);
}