#include "UpdateCallback.h"

#include "Helper.h"

#include <osg/NodeVisitor>

using namespace onep;

UpdateCallback::UpdateCallback()
	: NodeCallback(),
	  m_lastSimTime(0.0),
	  m_globe_angle(0.0)
{
}

void UpdateCallback::operator()(Node* node, NodeVisitor* nv)
{
	double time = nv->getFrameStamp()->getSimulationTime();
	double time_diff = 0.0;

	if (m_lastSimTime > 0.0)
	{
		time_diff = time - m_lastSimTime;
	}

	m_lastSimTime = time;


	m_globe_angle = fmodf(m_globe_angle + (float)time_diff * 0.1f, (C_PI * 2.0f));
	m_globeTransform->setAttitude(getQuatFromEuler(0.0, 0.0, m_globe_angle));

	traverse(node, nv);
}

void UpdateCallback::setGlobeTransform(ref_ptr<PositionAttitudeTransform> transform)
{
	m_globeTransform = transform;
}