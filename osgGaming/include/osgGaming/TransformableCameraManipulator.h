#pragma once

#include <osgGA/CameraManipulator>

namespace osgGaming
{
	class TransformableCameraManipulator : public osgGA::CameraManipulator
	{
	public:
		TransformableCameraManipulator();

		virtual void updateCamera(osg::Camera &camera);

		virtual osg::Matrixd getMatrix() const;
		virtual osg::Matrixd getInverseMatrix() const;

		virtual void setByMatrix(const osg::Matrixd &matrix);
		virtual void setByInverseMatrix(const osg::Matrixd &matrix);

		osg::Vec3f getPosition();
		osg::Quat getAttitude();

		void setPosition(osg::Vec3f position);
		void setAttitude(osg::Quat rotation);

		//void move(Vec3f vec);

		void updateProjectionRatio(double ratio);
		void updateClearColor(osg::Vec4 clearColor);

	private:
		osg::Vec3f _position;
		osg::Quat _attitude;
		osg::Vec4 _clearColor;

		double _projectionAngle;
		double _projectionNear;
		double _projectionFar;
		double _projectionRatio;

		bool _updateProjectionMatrix;
		bool _updateClearColor;

		osg::Matrixd _transformation;
	};
}