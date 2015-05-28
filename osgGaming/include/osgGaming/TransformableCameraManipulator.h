#pragma once

#include <vector>

#include <osgGaming/CameraAlignedQuad.h>

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

		void addCameraAlignedQuad(osg::ref_ptr<CameraAlignedQuad> caq);
		void removeCameraAlignedQuad(osg::ref_ptr<CameraAlignedQuad> caq);

		osg::Vec3f getPosition();
		osg::Quat getAttitude();

		osg::Matrix getViewMatrix();
		osg::Matrix getProjectionMatrix();
		double getProjectionRatio();

		// TODO: deprecated - move near far to viewer
		double getProjectionNear();
		double getProjectionFar();

		osg::Vec3f getLookDirection();

		void setPosition(osg::Vec3f position);
		void setAttitude(osg::Quat rotation);

		void updateResolution(float width, float height);
		void updateClearColor(osg::Vec4 clearColor);

		void getPickRay(float x, float y, osg::Vec3f& point, osg::Vec3f& direction);

	private:
		typedef std::vector<osg::ref_ptr<CameraAlignedQuad>> CameraAlignedQuadList;

		void updateViewMatrix();
		void updateCameraAlignedQuads();

		osg::Vec3f _position;
		osg::Quat _attitude;
		osg::Vec4f _clearColor;
		osg::Vec3f _look;

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

		CameraAlignedQuadList _cameraAlignedQuads;

		osg::Vec3f _v[4];
		osg::Vec3f _n;
		osg::Vec3f _v_res[4];
		osg::Vec3f _n_res[4];
	};
}