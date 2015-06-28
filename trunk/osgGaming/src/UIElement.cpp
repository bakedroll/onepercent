#include <osgGaming/UIElement.h>
#include <osgGaming/GameException.h>

#include <osg/Geometry>
#include <osg/Geode>

using namespace osg;
using namespace osgGaming;

UIElement::UIElement()
	: MatrixTransform(),
	  _width(/*10*/0.0f),
	  _height(/*10*/0.0f),
	  _padding(0.0f, 0.0f, 0.0f, 0.0f),
	  _margin(0.0f, 0.0f, 0.0f, 0.0f),
	  _horizontalAlignment(H_STRETCH),
	  _verticalAlignment(V_STRETCH),
	  _calculatedMinContentSize(false),
	  _visible(true)
	
{
	_visualSwitch = new Switch();
	MatrixTransform::addChild(_visualSwitch);
}

bool UIElement::addChild(Node* child)
{
	throw GameException("Invalid operation: UIElement::addChild()");
}

bool UIElement::insertChild(unsigned int index, Node* child)
{
	throw GameException("Invalid operation: UIElement::insertChild()");
}

bool UIElement::replaceChild(Node* origChild, Node* newChild)
{
	throw GameException("Invalid operation: UIElement::replaceChild()");
}

bool UIElement::setChild(unsigned int i, Node *node)
{
	throw GameException("Invalid operation: UIElement::setChild()");
}

Vec2f UIElement::getOrigin()
{
	return _origin;
}

Vec2f UIElement::getAbsoluteOrigin()
{
	return _absoluteOrigin;
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

Vec2f UIElement::getMinContentSize()
{
	if (_calculatedMinContentSize == false)
	{
		_minContentSize = calculateMinContentSize();

		_minContentSize.x() = fmaxf(_minContentSize.x(), _width);
		_minContentSize.y() = fmaxf(_minContentSize.y(), _height);

		_calculatedMinContentSize = true;
	}

	return _minContentSize;
}

Vec2f UIElement::getMinSize()
{
	return getMinContentSize() + Vec2f(
		_margin.x() + _margin.z() + _padding.x() + _padding.z(),
		_margin.y() + _margin.w() + _padding.y() + _padding.w());
}

bool UIElement::getVisible()
{
	return _visible;
}

void UIElement::setOrigin(Vec2f origin)
{
	_origin = origin;

	Matrix mat = Matrix::translate(_origin.x(), _origin.y(), 0.0f);
	setMatrix(mat);
}

void UIElement::setAbsoluteOrigin(Vec2f origin)
{
	_absoluteOrigin = origin;
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

void UIElement::setVisible(bool visible)
{
	if (visible)
	{
		_visualSwitch->setAllChildrenOn();
	}
	else
	{
		_visualSwitch->setAllChildrenOff();
	}

	_visible = visible;
}

void UIElement::resetMinContentSize()
{
	_calculatedMinContentSize = false;

	onResetMinContentSize();
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

		_visualSwitch->addChild(_visualGroup);

		updateVisualGroup();
	}

	return _visualGroup;
}

Vec2f UIElement::calculateMinContentSize()
{
	return Vec2f(0.0f, 0.0f);
}

void UIElement::onResetMinContentSize()
{

}

ref_ptr<Group> UIElement::getChildGroup()
{
	return _visualSwitch;
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