#pragma once

#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>

using namespace osgViewer;
using namespace osgGA;

namespace onep
{

	class GlobeViewer : public Viewer
	{
	public:
		GlobeViewer();

		void removeEventHandler(GUIEventHandler* eventHandler);
	};

}