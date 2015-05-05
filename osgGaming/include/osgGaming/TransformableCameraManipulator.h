#pragma once

#include <osgGA/CameraManipulator>

namespace osgGaming
{
	class TransformableCameraManipulator : public osgGA::CameraManipulator
	{
	public:
		TransformableCameraManipulator();

		virtual void updateCamera(osg::Camera &camera) override;

		virtual osg::Matrixd getMatrix() const override;
		virtual osg::Matrixd getInverseMatrix() const override;

		virtual void setByMatrix(const osg::Matrixd &matrix) override;
		virtual void setByInverseMatrix(const osg::Matrixd &matrix) override;

		osg::Vec3f getPosition();
		osg::Quat getAttitude();

		osg::Matrix getViewMatrix();
		osg::Matrix getProjectionMatrix();
		double getProjectionRatio();

		void setPosition(osg::Vec3f position);
		void setAttitude(osg::Quat rotation);

		void updateResolution(osg::Vec2f resolution);
		void updateClearColor(osg::Vec4 clearColor);

		void getPickRay(float x, float y, osg::Vec3f& point, osg::Vec3f& direction);

	private:
		void updateViewMatrix();

		osg::Vec3f _position;
		osg::Quat _attitude;
		osg::Vec4 _clearColor;

		osg::Vec2f _resolution;

		double _projectionAngle;
		double _projectionNear;
		double _projectionFar;
		double _projectionRatio;

		bool _updateViewMatrix;
		bool _updateClearColor;

		osg::Matrixd _transformation;

		osg::Matrixd _viewMatrix;
		osg::Matrixd _projectionMatrix;
	};
}