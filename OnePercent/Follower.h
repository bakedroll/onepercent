#pragma once

#include <osg/Camera>

namespace onep
{
	class Follower : public osg::Camera
	{
	public:
		Follower();

		osg::Vec3f getPosition();
		osg::Quat getAttitude();

		void setPosition(osg::Vec3f position);
		void setAttitude(osg::Quat rotation);

		//void move(Vec3f vec);

		void updateLookAtMatrix();
	private:
		osg::Matrix m_transformation;
		osg::Vec3f m_position;
		osg::Quat m_attitude;
	};
}