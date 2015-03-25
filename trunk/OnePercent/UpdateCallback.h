#pragma once

#include <osg/PositionAttitudeTransform>

using namespace osg;

namespace onep
{

	class UpdateCallback : public NodeCallback
	{
	public:
		UpdateCallback();

		virtual void operator()(Node* node, NodeVisitor* nv);

		void setGlobeTransform(ref_ptr<PositionAttitudeTransform> transform);

	private:
		double m_lastSimTime;

		float m_globe_angle;

		ref_ptr<PositionAttitudeTransform> m_globeTransform;
	};

}