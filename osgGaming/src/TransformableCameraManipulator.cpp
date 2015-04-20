#include <osgGaming/TransformableCameraManipulator.h>
#include <osgGaming/Helper.h>

using namespace osgGaming;
using namespace osgGA;
using namespace osg;

TransformableCameraManipulator::TransformableCameraManipulator()
	: osgGA::CameraManipulator(),
	_updateClearColor(true),
	_updateProjectionMatrix(false),
	_projectionAngle(45.0),
	_projectionNear(1.0),
	_projectionFar(20000.0),
	_projectionRatio(0.0),
	_clearColor(Vec4(0.0, 0.0, 0.0, 1.0))
{

}

void TransformableCameraManipulator::updateCamera(osg::Camera &camera)
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

	camera.setViewMatrixAsLookAt(eye, center, up);

	if (_updateProjectionMatrix)
	{
		camera.setProjectionMatrixAsPerspective(
			_projectionAngle,
			_projectionRatio,
			_projectionNear,
			_projectionFar);

		_updateProjectionMatrix = false;
	}

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

void TransformableCameraManipulator::updateProjectionRatio(double ratio)
{
	_projectionRatio = ratio;
	_updateProjectionMatrix = true;
}

void TransformableCameraManipulator::updateClearColor(Vec4 clearColor)
{
	_clearColor = clearColor;
	_updateClearColor = true;
}

/*void Follower::move(Vec3f vec)
{
transformVector(&vec, &m_transformation);
m_position = vec;
m_transformation.setTrans(m_position);
}*/

Vec3f TransformableCameraManipulator::getPosition()
{
	return _position;
}

Quat TransformableCameraManipulator::getAttitude()
{
	return _attitude;
}

void TransformableCameraManipulator::setPosition(Vec3f position)
{
	_position = position;
}

void TransformableCameraManipulator::setAttitude(Quat attitude)
{
	_attitude = attitude;
}