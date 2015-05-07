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
	_v[0] = Vec3f(-1.0f, -1.0f, -1.0f);
	_v[1] = Vec3f(-1.0f, 1.0f, -1.0f);
	_v[2] = Vec3f(1.0f, 1.0f, -1.0f);
	_v[3] = Vec3f(1.0f, -1.0f, -1.0f);

	_n = Vec3f(0.0f, 0.0f, 1.0f);
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

	updateCameraAlignedQuads();
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

void TransformableCameraManipulator::addCameraAlignedQuad(ref_ptr<CameraAlignedQuad> caq)
{
	_cameraAlignedQuads.push_back(caq);
}

void TransformableCameraManipulator::removeCameraAlignedQuad(ref_ptr<CameraAlignedQuad> caq)
{
	for (CameraAlignedQuadList::iterator it = _cameraAlignedQuads.begin(); it != _cameraAlignedQuads.end(); ++it)
	{
		if ((*it) == caq)
		{
			_cameraAlignedQuads.erase(it);
			return;
		}
	}
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

	_look = center;

	up -= _position;

	_viewMatrix = Matrix::lookAt(eye, center, up);

	_updateViewMatrix = false;
}

void TransformableCameraManipulator::updateCameraAlignedQuads()
{
	if (_cameraAlignedQuads.size() == 0)
	{
		return;
	}

	Matrixd mat = Matrix::inverse(_viewMatrix * _projectionMatrix);

	for (int i = 0; i < 4; i++)
	{
		_v_res[i] = _v[i] * mat;
		_n_res[i] = ((_v[i] + _n) * mat) - _v_res[i];
		_n_res[i].normalize();
	}

	for (CameraAlignedQuadList::iterator it = _cameraAlignedQuads.begin(); it != _cameraAlignedQuads.end(); ++it)
	{
		ref_ptr<Vec3Array> verts = it->get()->getVertexArray();
		ref_ptr<Vec3Array> normals = it->get()->getNormalArray();

		for (int i = 0; i < 4; i++)
		{
			verts->at(i).set(_v_res[i]);
			normals->at(i).set(_n_res[i]);
		}
		
		normals->dirty();
		verts->dirty();
		it->get()->getGeometry()->dirtyBound();
	}
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

Vec3f TransformableCameraManipulator::getLookDirection()
{
	return _look;
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