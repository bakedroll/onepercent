#include "GameApplication.h"

#include "GlobeModel.h"

#include "Follower.h"
#include "GlobeViewer.h"

#include <osg/PositionAttitudeTransform>

using namespace onep;
using namespace osg;

int GameApplication::run()
{
	GlobeViewer viewer;

	ref_ptr<Group> group = new Group();

	ref_ptr<GlobeModel> globe = new GlobeModel();
	group->addChild(globe);


	/*ref_ptr<Geode> box_geode = new Geode();
	ref_ptr<Geode> globe_geode = new Geode();

	ref_ptr<ShapeDrawable> box = new ShapeDrawable(new Box(Vec3(3.0, 0, 0), 1, 1, 1));
	ref_ptr<GlobeModel> globe = new GlobeModel();

	box_geode->addDrawable(box);
	globe_geode->addDrawable(globe);

	group->addChild(box_geode);
	group->addChild(globe_geode);*/

	group->getOrCreateStateSet()->setMode(GL_CULL_FACE, StateAttribute::ON);
	group->getStateSet()->setMode(GL_NORMALIZE, StateAttribute::ON);
	group->getStateSet()->setMode(GL_DEPTH_TEST, StateAttribute::ON);
	group->getStateSet()->setMode(GL_LIGHTING, StateAttribute::ON);

	group->getStateSet()->setMode(GL_LIGHT0 , StateAttribute::ON);
	group->getStateSet()->setMode(GL_LIGHT1, StateAttribute::ON);

	ref_ptr<Light> light1 = new Light();
	ref_ptr<LightSource> ls1 = new LightSource();

	ref_ptr<Light> light2 = new Light();
	ref_ptr<LightSource> ls2 = new LightSource();

	light1->setLightNum(0);
	light1->setDiffuse(Vec4(1.0, 0.7, 0.3, 1.0));
	light1->setSpecular(Vec4(1.0, 1.0, 1.0, 1.0));
	light1->setAmbient(Vec4(0.0, 0.0, 0.0, 1.0));
	light1->setPosition(Vec4(0.0, 0.0, 0.0, 1.0));

	ls1->setLight(light1);
	//ls->setLocalStateSetModes(StateAttribute::ON);

	light2->setLightNum(1);
	light2->setDiffuse(Vec4(1.0, 1.0, 1.0, 1.0));
	light2->setSpecular(Vec4(1.0, 1.0, 1.0, 1.0));
	light2->setAmbient(Vec4(0.0, 0.0, 0.0, 1.0));
	light2->setPosition(Vec4(-1.0, -0.5, 0.0, 0.0));

	ls2->setLight(light2);

	ref_ptr<PositionAttitudeTransform> transform = new PositionAttitudeTransform();
	transform->setPosition(Vec3(5, 3, 0));
	transform->addChild(ls1);

	group->addChild(transform);
	group->addChild(ls2);

	viewer.setSceneData(group);

	ref_ptr<Follower> follower = new Follower();
	//viewer.setCamera(follower);

	follower->setPosition(Vec3(0, -4, 0));
	follower->updateLookAtMatrix();

	//viewer.setCameraManipulator(new osgGA::TrackballManipulator());

	/*while (!viewer.done())
	{
		viewer.frame();
	}*/

	viewer.setUpViewInWindow(10, 50, 1280, 768);

	viewer.run();

	return 0;
}