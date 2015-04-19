#include "EventManager.h"

using namespace onep;

bool EventManager::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) :
	{
		printf("Mouse x: %f", ea.getX());
	}
	default:
		return false;
	}
}