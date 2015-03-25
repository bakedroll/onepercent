#include "GameApplication.h"

#include "GlobeModel.h"
#include "Helper.h"
#include "Follower.h"
#include "GlobeViewer.h"
#include "UpdateCallback.h"

#include <osg/PositionAttitudeTransform>
#include <osg/LightModel>

using namespace onep;
using namespace osg;

int GameApplication::run()
{
	GlobeViewer viewer;

	ref_ptr<Group> group = new Group();

	ref_ptr<PositionAttitudeTransform> transform = new PositionAttitudeTransform();
	ref_ptr<GlobeModel> globe = new GlobeModel();

	transform->addChild(globe);
	group->addChild(transform);


	group->getOrCreateStateSet()->setMode(GL_CULL_FACE, StateAttribute::ON);
	group->getStateSet()->setMode(GL_NORMALIZE, StateAttribute::ON);
	group->getStateSet()->setMode(GL_DEPTH_TEST, StateAttribute::ON);
	group->getStateSet()->setMode(GL_LIGHTING, StateAttribute::ON);

	group->getStateSet()->setMode(GL_LIGHT0 , StateAttribute::ON);
	//group->getStateSet()->setMode(GL_LIGHT1, StateAttribute::ON);

	ref_ptr<LightModel> lightmodel = new LightModel();
	lightmodel->setAmbientIntensity(Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	group->getStateSet()->setAttributeAndModes(lightmodel, StateAttribute::ON);

	ref_ptr<Light> light1 = new Light();
	ref_ptr<LightSource> ls1 = new LightSource();



	/*ref_ptr<Light> light2 = new Light();
	ref_ptr<LightSource> ls2 = new LightSource();

	light2->setLightNum(1);
	light2->setDiffuse(Vec4(1.0, 0.7, 0.3, 1.0));
	light2->setSpecular(Vec4(1.0, 1.0, 1.0, 1.0));
	light2->setAmbient(Vec4(0.0, 0.0, 0.0, 1.0));
	light2->setPosition(Vec4(10.0, 5.0, 0.0, 1.0));

	ls2->setLight(light2);
	group->addChild(ls2);*/



	light1->setLightNum(0);
	light1->setDiffuse(Vec4(1.0, 1.0, 1.0, 1.0));
	light1->setSpecular(Vec4(1.0, 1.0, 1.0, 1.0));
	light1->setAmbient(Vec4(0.0, 0.0, 0.0, 1.0));
	light1->setPosition(Vec4(getVec3FromEuler(0.0, -23.5 * C_PI / 180.0, C_PI / 2.0), 0.0));

	ls1->setLight(light1);

	group->addChild(ls1);

	ref_ptr<UpdateCallback> update_callback = new UpdateCallback();
	update_callback->setGlobeTransform(transform);

	group->setUpdateCallback(update_callback);

	viewer.setSceneData(group);

	/*ref_ptr<Follower> follower = new Follower();
	viewer.setCamera(follower);

	follower->setPosition(Vec3(0, -4, 0));
	follower->updateLookAtMatrix();

	while (!viewer.done())
	{
		viewer.frame();
	}*/

	viewer.setUpViewInWindow(10, 50, 1280, 768);

	viewer.getCamera()->setClearColor(Vec4(0.0, 0.0, 0.0, 1.0));

	viewer.run();

	return 0;
}