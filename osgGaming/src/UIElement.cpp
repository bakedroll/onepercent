#include <osgGaming/UIElement.h>

#include <osg/Geometry>
#include <osg/Geode>

using namespace osg;
using namespace osgGaming;

UIElement::UIElement()
	: MatrixTransform(),
	  _width(100.0f),
	  _height(100.0f),
	  _padding(0.0f, 0.0f, 0.0f, 0.0f),
	  _margin(0.0f, 0.0f, 0.0f, 0.0f),
	  _horizontalAlignment(H_STRETCH),
	  _verticalAlignment(V_STRETCH),
	  _calculatedContentSize(false)
	
{

}

Vec2f UIElement::getOrigin()
{
	return _origin;
}

Vec2f UIElement::getSize()
{
	return _size;
}

Vec2f UIElement::getContentSize()
{
	return _size - Vec2f(
		_margin.x() + _margin.z() + _padding.x() + _padding.z(),
		_margin.y() + _margin.w() + _padding.y() + _padding.w());
}

float UIElement::getWidth()
{
	return _width;
}

float UIElement::getHeight()
{
	return _height;
}


Vec4f UIElement::getPadding()
{
	return _padding;
}

Vec4f UIElement::getMargin()
{
	return _margin;
}


UIElement::HorizontalAlignment UIElement::getHorizontalAlignment()
{
	return _horizontalAlignment;
}

UIElement::VerticalAlignment UIElement::getVerticalAlignment()
{
	return _verticalAlignment;
}

void UIElement::getOriginSizeForChildInArea(unsigned int i, Vec2f area, Vec2f& origin, Vec2f& size)
{
	origin = Vec2f(0.0f, 0.0f);
	size = area;
}

Vec2f UIElement::getMinContentSize()
{
	if (_calculatedContentSize == false)
	{
		_minContentSize = calculateMinContentSize();
		_calculatedContentSize = true;
	}

	return _minContentSize;
}

Vec2f UIElement::getMinSize()
{
	return getMinContentSize() + Vec2f(
		_margin.x() + _margin.z() + _padding.x() + _padding.z(),
		_margin.y() + _margin.w() + _padding.y() + _padding.w());
}

UIElement::UIElementList UIElement::getUIChildren()
{
	UIElement::UIElementList elements;

	unsigned int num = getNumChildren();
	for (unsigned int i = 0; i < num; i++)
	{
		ref_ptr<UIElement> element = dynamic_cast<UIElement*>(getChild(i));
		if (element.valid())
		{
			elements.push_back(element);
		}
	}

	return elements;
}

unsigned int UIElement::getNumUIChildren()
{
	unsigned int result = 0;

	unsigned int num = getNumChildren();
	for (unsigned int i = 0; i < num; i++)
	{
		ref_ptr<UIElement> element = dynamic_cast<UIElement*>(getChild(i));
		if (element.valid())
		{
			result++;
		}
	}

	return result;
}

void UIElement::setOrigin(Vec2f origin)
{
	_origin = origin;

	Matrix mat = Matrix::translate(_origin.x(), _origin.y(), 0.0f);
	setMatrix(mat);
}

void UIElement::setSize(Vec2f size)
{
	_size = size;

	updateVisualGroup();
}

void UIElement::setWidth(float width)
{
	_width = width;
}

void UIElement::setHeight(float height)
{
	_height = height;
}


void UIElement::setPadding(osg::Vec4f padding)
{
	_padding = padding;
}

void UIElement::setMargin(osg::Vec4f margin)
{
	_margin = margin;
}

void UIElement::setPadding(float padding)
{
	_padding = Vec4(padding, padding, padding, padding);
}

void UIElement::setMargin(float margin)
{
	_margin = Vec4(margin, margin, margin, margin);
}

void UIElement::setHorizontalAlignment(UIElement::HorizontalAlignment alignment)
{
	_horizontalAlignment = alignment;
}

void UIElement::setVerticalAlignment(UIElement::VerticalAlignment alignment)
{
	_verticalAlignment = alignment;
}

void UIElement::resetMinContentSize()
{
	_calculatedContentSize = false;

	resetChildrenMinContentSize();
}

void UIElement::updatedContentOriginSize(Vec2f origin, Vec2f size)
{

}

ref_ptr<Group> UIElement::getVisualGroup()
{
	if (!_visualGroup.valid())
	{
		_visualGroup = new MatrixTransform();

		ref_ptr<Geode> geode = new Geode();
		ref_ptr<Geometry> geo = new Geometry();

		ref_ptr<Vec3Array> verts = new Vec3Array();
		verts->push_back(Vec3(0.0f, 0.0f, -1.0f));
		verts->push_back(Vec3(1.0f, 0.0f, -1.0f));
		verts->push_back(Vec3(1.0f, 1.0f, -1.0f));
		verts->push_back(Vec3(0.0f, 1.0f, -1.0f));

		ref_ptr<DrawElementsUInt> indices = new DrawElementsUInt(PrimitiveSet::LINE_LOOP, 0);
		indices->push_back(0);
		indices->push_back(1);
		indices->push_back(2);
		indices->push_back(3);

		ref_ptr<Vec4Array> colors = new Vec4Array();
		colors->push_back(Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		ref_ptr<Vec2Array> texcoords = new Vec2Array();
		texcoords->push_back(Vec2(0.0f, 0.0f));
		texcoords->push_back(Vec2(1.0f, 0.0f));
		texcoords->push_back(Vec2(1.0f, 1.0f));
		texcoords->push_back(Vec2(0.0f, 1.0f));

		geo->setTexCoordArray(0, texcoords);
		geo->addPrimitiveSet(indices);
		geo->setVertexArray(verts);
		geo->setColorArray(colors);
		geo->setColorBinding(osg::Geometry::BIND_OVERALL);

		_visualGroup->addChild(geode);
		geode->addDrawable(geo);

		addChild(_visualGroup);

		updateVisualGroup();
	}

	return _visualGroup;
}

Vec2f UIElement::calculateMinContentSize()
{
	return Vec2f(0.0f, 0.0f);
}

void UIElement::resetChildrenMinContentSize()
{

}

void UIElement::updateVisualGroup()
{
	if (_visualGroup.valid())
	{
		Matrix matTrans = Matrix::translate(_margin.x(), _margin.w(), 0.0f);
		Matrix matScale = Matrix::scale(_size.x() - (_margin.x() + _margin.z()), _size.y() - (_margin.y() + _margin.w()), 1.0f);

		_visualGroup->setMatrix(matScale * matTrans);
	}
}