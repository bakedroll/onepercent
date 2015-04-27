#include <osgGaming/UIStackPanel.h>

using namespace osgGaming;
using namespace osg;

UIStackPanel::UIStackPanel()
	: UIContainerElement(),
	  _spacing(5.0f)
{
	_cells = new UICells();
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

void UIStackPanel::setSpacing(float spacing)
{
	_spacing = spacing;
}

void UIStackPanel::setOrientation(Orientation orientation)
{
	_orientation = orientation;
}

void UIStackPanel::setNumCells(int num)
{
	_numCells = num;
}

Vec2f UIStackPanel::calculateMinContentSize()
{
	_cells->setNumCells(_numCells);

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


	float minContentOrientationSize = ((float)(_numCells - 1) * _spacing);

	for (int i = 0; i < _numCells; i++)
	{
		float size;
		_cells->getMinSize(i, size);

		minContentOrientationSize += size;
	}

	return _orientation == HORIZONTAL ?
		Vec2f(minContentOrientationSize, minLaneSize) :
		Vec2f(minLaneSize, minContentOrientationSize);
}