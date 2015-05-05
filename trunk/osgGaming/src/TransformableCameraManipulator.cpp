#include <osgGaming/TransformableCameraManipulator.h>
#include <osgGaming/Helper.h>

using namespace osgGaming;
using namespace osgGA;
using namespace osg;

TransformableCameraManipulator::TransformableCameraManipulator()
	: osgGA::CameraManipulator(),
	_updateClearColor(true),
	_updateViewMatrix(true),
	_projectionAngle(30.0),
	_projectionNear(0.2),
	_projectionFar(200.0),
	_projectionRatio(0.0),
	_clearColor(Vec4(0.0, 0.0, 0.0, 1.0))
{

}

void TransformableCameraManipulator::updateCamera(osg::Camera &camera)
{
	if (_updateViewMatrix)
	{
		updateViewMatrix();
	}

	camera.setViewMatrix(_viewMatrix);
	camera.setProjectionMatrix(_projectionMatrix);

	if (_updateClearColor)
	{
		camera.setClearColor(_clearColor);

		_updateClearColor = false;
	}
}

Matrixd TransformableCameraManipulator::getMatrix() const
{
	return Matrixd::identity();
}

Matrixd TransformableCameraManipulator::getInverseMatrix() const
{
	return Matrixd::identity();
}

void TransformableCameraManipulator::setByMatrix(const Matrixd &matrix)
{

}

void TransformableCameraManipulator::setByInverseMatrix(const Matrixd &matrix)
{

}

void TransformableCameraManipulator::updateResolution(Vec2f resolution)
{
	_resolution = resolution;
	_projectionRatio = _resolution.x() / _resolution.y();

	_projectionMatrix = Matrix::perspective(
		_projectionAngle,
		_projectionRatio,
		_projectionNear,
		_projectionFar);
}

void TransformableCameraManipulator::updateClearColor(Vec4 clearColor)
{
	_clearColor = clearColor;
	_updateClearColor = true;
}

void TransformableCameraManipulator::getPickRay(float x, float y, osg::Vec3f& point, osg::Vec3f& direction)
{
	float mappedX = (x * 2.0f) / _resolution.x() - 1.0f;
	float mappedY = (y * 2.0f) / _resolution.y() - 1.0f;

	Vec3f near(mappedX, mappedY, -1.0f);
	Vec3f far(mappedX, mappedY, 1.0f);

	Matrixd mat = Matrix::inverse(_viewMatrix * _projectionMatrix);

	point = near * mat;
	direction = (far * mat) - point;

	direction.normalize();
}

void TransformableCameraManipulator::updateViewMatrix()
{
	Vec3 eye(0.0f, 0.0f, 0.0f);
	Vec3 center(0.0f, 1.0f, 0.0f);
	Vec3 up(0.0f, 0.0f, 1.0f);

	_transformation.setRotate(_attitude);
	_transformation.setTrans(_position);

	transformVector(&eye, &_transformation);
	transformVector(&center, &_transformation);
	transformVector(&up, &_transformation);

	up -= _position;

	_viewMatrix = Matrix::lookAt(eye, center, up);

	_updateViewMatrix = false;
}

Vec3f TransformableCameraManipulator::getPosition()
{
	return _position;
}

Quat TransformableCameraManipulator::getAttitude()
{
	return _attitude;
}

Matrix TransformableCameraManipulator::getViewMatrix()
{
	if (_updateViewMatrix)
	{
		updateViewMatrix();
	}

	return _viewMatrix;
}

Matrix TransformableCameraManipulator::getProjectionMatrix()
{
	return _projectionMatrix;
}

double TransformableCameraManipulator::getProjectionRatio()
{
	return _projectionRatio;
}

void TransformableCameraManipulator::setPosition(Vec3f position)
{
	_position = position;

	_updateViewMatrix = true;
}

void TransformableCameraManipulator::setAttitude(Quat attitude)
{
	_attitude = attitude;

	_updateViewMatrix = true;
}