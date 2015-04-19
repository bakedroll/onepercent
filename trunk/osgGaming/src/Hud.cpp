#include <osgGaming/Hud.h>

#include <osg/MatrixTransform>
#include <osgText/Text>

using namespace osgGaming;
using namespace osgText;
using namespace osg;

Hud::Hud()
	: Referenced()
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



	/*osgText::Text* textOne = new osgText::Text();
	textOne->setCharacterSize(25);
	// textOne->setFont("C:/WINDOWS/Fonts/impact.ttf");
	textOne->setText("Not so good");
	textOne->setAxisAlignment(osgText::Text::SCREEN);
	textOne->setPosition(osg::Vec3(360, 165, -1.5));
	textOne->setColor(osg::Vec4(199, 77, 15, 1));*/


	_geode = new Geode();
	_geode->setStateSet(stateSet);
	//hudGeode->addDrawable(textOne);

	ref_ptr<MatrixTransform> modelViewMatrix = new MatrixTransform();
	modelViewMatrix->setMatrix(Matrix::identity());
	modelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	modelViewMatrix->addChild(_geode);

	_projection = new Projection();
	_projection->setMatrix(Matrix::ortho2D(0, 1280, 0, 768));
	_projection->addChild(modelViewMatrix);
}

ref_ptr<Projection> Hud::getProjection()
{
	return _projection;
}

ref_ptr<Geode> Hud::getGeode()
{
	return _geode;
}