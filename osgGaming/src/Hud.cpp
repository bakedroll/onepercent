#include <osgGaming/Hud.h>
#include <osgGaming/FpsTextCallback.h>
#include <osgGaming/UIGrid.h>
#include <osgGaming/UIUpdateVisitor.h>

#include <osg/MatrixTransform>

using namespace osgGaming;
using namespace osgText;
using namespace osg;

Hud::Hud()
	: Referenced(),
	_fpsEnabled(false)
{
	/*osg::Geometry* HUDBackgroundGeometry = new osg::Geometry();

	osg::Vec3Array* HUDBackgroundVertices = new osg::Vec3Array;
	HUDBackgroundVertices->push_back(osg::Vec3(0, 0, -1));
	HUDBackgroundVertices->push_back(osg::Vec3(1024, 0, -1));
	HUDBackgroundVertices->push_back(osg::Vec3(1024, 200, -1));
	HUDBackgroundVertices->push_back(osg::Vec3(0, 200, -1));

	osg::DrawElementsUInt* HUDBackgroundIndices =
		new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
	HUDBackgroundIndices->push_back(0);
	HUDBackgroundIndices->push_back(1);
	HUDBackgroundIndices->push_back(2);
	HUDBackgroundIndices->push_back(3);

	osg::Vec4Array* HUDcolors = new osg::Vec4Array;
	HUDcolors->push_back(osg::Vec4(0.8f, 0.8f, 0.8f, 0.8f));

	osg::Vec2Array* texcoords = new osg::Vec2Array(4);
	(*texcoords)[0].set(0.0f, 0.0f);
	(*texcoords)[1].set(1.0f, 0.0f);
	(*texcoords)[2].set(1.0f, 1.0f);
	(*texcoords)[3].set(0.0f, 1.0f);

	HUDBackgroundGeometry->setTexCoordArray(0, texcoords);

	osg::Vec3Array* HUDnormals = new osg::Vec3Array;
	HUDnormals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	HUDBackgroundGeometry->setNormalArray(HUDnormals);
	HUDBackgroundGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	HUDBackgroundGeometry->addPrimitiveSet(HUDBackgroundIndices);
	HUDBackgroundGeometry->setVertexArray(HUDBackgroundVertices);
	HUDBackgroundGeometry->setColorArray(HUDcolors);
	HUDBackgroundGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	hudGeode->addDrawable(HUDBackgroundGeometry);*/


	// Create and set up a state set using the texture from above:
	ref_ptr<StateSet> stateSet = new StateSet();
	stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateSet->setRenderBinDetails(11, "RenderBin");


	_geode = new Geode();

	_rootUIElement = new UIGrid();

	ref_ptr<MatrixTransform> modelViewMatrix = new MatrixTransform();
	modelViewMatrix->setMatrix(Matrix::identity());
	modelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	modelViewMatrix->addChild(_geode);
	modelViewMatrix->addChild(_rootUIElement);

	_projection = new Projection();
	_projection->addChild(modelViewMatrix);
	_projection->setStateSet(stateSet);
}

ref_ptr<Projection> Hud::getProjection()
{
	return _projection;
}

ref_ptr<Geode> Hud::getGeode()
{
	return _geode;
}

ref_ptr<UIElement> Hud::getRootUIElement()
{
	return _rootUIElement;
}

void Hud::updateResolution(unsigned int width, unsigned int height)
{
	_projection->setMatrix(Matrix::ortho2D(0.0, (double)width - 1.0, 1.0, (double)height - 1.0));

	_resolution = Vec2f((float)width, (float)height);

	updateUIElements();
}

void Hud::updateUIElements()
{
	_rootUIElement->setOrigin(Vec2f(0.0f, 0.0f));
	_rootUIElement->setSize(_resolution);

	UIUpdateVisitor updateVisitor;
	_rootUIElement->accept(updateVisitor);
}

void Hud::setFpsEnabled(bool enabled)
{
	if (enabled == _fpsEnabled)
	{
		return;
	}

	_fpsEnabled = enabled;

	if (!_fpsText.valid())
	{
		_fpsText = new osgText::Text();
		_fpsText->setCharacterSize(25);
		_fpsText->setFont("./data/fonts/coolvetica rg.ttf");
		_fpsText->setText("");
		_fpsText->setAxisAlignment(osgText::Text::SCREEN);
		_fpsText->setAlignment(osgText::TextBase::LEFT_BOTTOM);
		_fpsText->setPosition(osg::Vec3(10, 10, -1.5));
		_fpsText->setColor(osg::Vec4(199, 77, 15, 1));
		_fpsText->setDataVariance(osg::Object::DYNAMIC);
		_fpsText->setUpdateCallback(new FpsTextCallback());
	}

	if (_fpsEnabled == true)
	{
		_geode->addDrawable(_fpsText);
	}
	else
	{
		_geode->removeDrawable(_fpsText);
	}
}

void Hud::setRootUIElement(osg::ref_ptr<UIElement> element)
{
	_rootUIElement = element;
	updateUIElements();
}
