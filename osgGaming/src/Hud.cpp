#include <osgGaming/Hud.h>
#include <osgGaming/FpsTextCallback.h>
#include <osgGaming/UIGrid.h>
#include <osgGaming/UIUpdateVisitor.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/Helper.h>

#include <osg/MatrixTransform>

using namespace osgGaming;
using namespace osgText;
using namespace osg;

Hud::Hud()
	: Referenced(),
	_fpsEnabled(false)
{
	ref_ptr<StateSet> stateSet = new StateSet();
	stateSet->setMode(GL_BLEND, StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	stateSet->setMode(GL_LIGHTING, StateAttribute::OFF);
	stateSet->setRenderingHint(StateSet::TRANSPARENT_BIN);
	stateSet->setRenderBinDetails(11, "RenderBin");


	_geode = new Geode();

	_rootUIElement = new UIGrid();

	ref_ptr<MatrixTransform> modelViewMatrix = new MatrixTransform();
	modelViewMatrix->setMatrix(Matrix::identity());
	modelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	modelViewMatrix->addChild(_geode);
	modelViewMatrix->addChild(_rootUIElement);

	_projection = new Projection();
	_projection->addChild(modelViewMatrix);
	_projection->setStateSet(stateSet);
}

ref_ptr<Projection> Hud::getProjection()
{
	return _projection;
}

ref_ptr<Geode> Hud::getGeode()
{
	return _geode;
}

ref_ptr<UIElement> Hud::getRootUIElement()
{
	return _rootUIElement;
}

Hud::UIMList Hud::getUserInteractionModels()
{
	return _uimList;
}

void Hud::updateResolution(unsigned int width, unsigned int height)
{
	_projection->setMatrix(Matrix::ortho2D(0.0, (double)width - 1.0, 1.0, (double)height - 1.0));

	_resolution = Vec2f((float)width, (float)height);

	updateUIElements();
}

void Hud::updateUIElements()
{
	Vec2f origin(0.0f, 0.0f);

	_rootUIElement->setAbsoluteOrigin(origin);
	_rootUIElement->setOrigin(origin);
	_rootUIElement->setSize(_resolution);

	UIUpdateVisitor updateVisitor;
	_rootUIElement->accept(updateVisitor);
}

void Hud::setFpsEnabled(bool enabled)
{
	if (enabled == _fpsEnabled)
	{
		return;
	}

	_fpsEnabled = enabled;

	if (!_fpsText.valid())
	{
		_fpsText = createTextNode("", 25, ResourceManager::getInstance()->loadDefaultFont());
		_fpsText->setAlignment(osgText::TextBase::LEFT_BOTTOM);
		_fpsText->setPosition(osg::Vec3(10, 10, -1.5));
		_fpsText->setDataVariance(osg::Object::DYNAMIC);
		_fpsText->setUpdateCallback(new FpsTextCallback());
	}

	if (_fpsEnabled == true)
	{
		_geode->addDrawable(_fpsText);
	}
	else
	{
		_geode->removeDrawable(_fpsText);
	}
}

void Hud::setRootUIElement(osg::ref_ptr<UIElement> element)
{
	_rootUIElement = element;
	updateUIElements();
}

void Hud::registerUserInteractionModel(UserInteractionModel* model)
{
	_uimList.push_back(model);
}

bool Hud::anyUserInteractionModelHovered()
{
	for (UIMList::iterator it = _uimList.begin(); it != _uimList.end(); ++it)
	{
		if ((*it)->getHovered())
		{
			return true;
		}
	}

	return false;
}

void Hud::resetUserInteractionModel()
{
	for (UIMList::iterator it = _uimList.begin(); it != _uimList.end(); ++it)
	{
		(*it)->setHovered(false);
	}
}