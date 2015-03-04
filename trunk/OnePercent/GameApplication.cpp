#include "GameApplication.h"

#include "GlobeModel.h"

#include "Follower.h"
#include "GlobeViewer.h"

#include <osg/ShapeDrawable>
#include <osgUtil/GLObjectsVisitor>

using namespace onep;
using namespace osg;

int GameApplication::run()
{
	GlobeViewer viewer;

	ref_ptr<Group> group = new Group();
	ref_ptr<Geode> box_geode = new Geode();
	ref_ptr<Geode> globe_geode = new Geode();

	ref_ptr<ShapeDrawable> box = new ShapeDrawable(new Box(Vec3(3.0, 0, 0), 1, 1, 1));
	ref_ptr<GlobeModel> globe = new GlobeModel();

	box_geode->addDrawable(box);
	globe_geode->addDrawable(globe);

	group->addChild(box_geode);
	group->addChild(globe_geode);

	group->getOrCreateStateSet()->setMode(GL_CULL_FACE, StateAttribute::ON);
	group->getStateSet()->setMode(GL_NORMALIZE, StateAttribute::ON);
	group->getStateSet()->setMode(GL_DEPTH_TEST, StateAttribute::ON);
	group->getStateSet()->setMode(GL_LIGHTING, StateAttribute::ON);

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

	viewer.setUpViewInWindow(0, 0, 1280, 768);

	viewer.run();

	return 0;
}