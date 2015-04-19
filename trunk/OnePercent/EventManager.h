#pragma once

#include <osgGA/GUIEventHandler>

using namespace osg;

namespace onep
{
	class EventManager : public osgGA::GUIEventHandler
	{
	public:

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&);
		virtual void accept(osgGA::GUIEventHandlerVisitor& v)   { v.visit(*this); };
	};
}