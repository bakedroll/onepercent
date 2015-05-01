#include <osgGaming/UIButton.h>

#include <osg/Geode>
#include <osg/Geometry>

using namespace osgGaming;
using namespace osg;

UIButton::UIButton()
	: UIVisualElement(),
	  UserInteractionModel()
{
	ref_ptr<Geode> geode = new Geode();
	ref_ptr<Geometry> geo = new Geometry();

	ref_ptr<Vec3Array> verts = new Vec3Array();
	verts->push_back(Vec3(0.0f, 0.0f, -1.0f));
	verts->push_back(Vec3(1.0f, 0.0f, -1.0f));
	verts->push_back(Vec3(1.0f, 1.0f, -1.0f));
	verts->push_back(Vec3(0.0f, 1.0f, -1.0f));

	ref_ptr<DrawElementsUInt> indices = new DrawElementsUInt(PrimitiveSet::POLYGON, 0);
	indices->push_back(0);
	indices->push_back(1);
	indices->push_back(2);
	indices->push_back(3);

	//ref_ptr<Vec4Array> colors = new Vec4Array();
	//colors->push_back(Vec4(1.0f, 0.5f, 0.5f, 1.0f));

	ref_ptr<Vec2Array> texcoords = new Vec2Array();
	texcoords->push_back(Vec2(0.0f, 0.0f));
	texcoords->push_back(Vec2(1.0f, 0.0f));
	texcoords->push_back(Vec2(1.0f, 1.0f));
	texcoords->push_back(Vec2(0.0f, 1.0f));

	geo->setTexCoordArray(0, texcoords);
	geo->addPrimitiveSet(indices);
	geo->setVertexArray(verts);
	//geo->setColorArray(colors);
	//geo->setColorBinding(osg::Geometry::BIND_OVERALL);

	geode->addDrawable(geo);

	_material = new Material();
	_material->setAmbient(Material::FRONT_AND_BACK, Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	_material->setSpecular(Material::FRONT_AND_BACK, Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	_material->setEmission(Material::FRONT_AND_BACK, Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	_material->setDiffuse(Material::FRONT_AND_BACK, Vec4(1.0f, 0.5f, 0.5f, 1.0f));
	_material->setTransparency(Material::FRONT_AND_BACK, 0.5f);
	geode->getOrCreateStateSet()->setAttributeAndModes(_material, StateAttribute::ON);

	getVisualGroup()->addChild(geode);
}

void UIButton::onMouseEnter()
{
	_material->setTransparency(Material::FRONT_AND_BACK, 0.0f);
}

void UIButton::onMouseLeave()
{
	_material->setTransparency(Material::FRONT_AND_BACK, 0.5f);
}

void UIButton::getAbsoluteOriginSize(osg::Vec2f& origin, osg::Vec2f& size)
{
	origin = getAbsoluteOrigin();
	size = getContentSize();
}