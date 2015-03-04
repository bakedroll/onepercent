#include "GlobeViewer.h"

using namespace onep;

GlobeViewer::GlobeViewer()
	: osgViewer::Viewer()
{
}

void GlobeViewer::removeEventHandler(GUIEventHandler* eventHandler)
{
	osgViewer::View::EventHandlers::iterator it = std::find(_eventHandlers.begin(), _eventHandlers.end(), eventHandler);
	if (it != _eventHandlers.end())
		_eventHandlers.erase(it);
}