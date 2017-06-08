#include <osgGaming/Hud.h>
#include <osgGaming/FpsTextCallback.h>
#include <osgGaming/UIUpdateVisitor.h>
#include <osgGaming/UIMCollectorVisitor.h>
#include <osgGaming/UIFindElementVisitor.h>
#include <osgGaming/UIMarkupLoader.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/Helper.h>

#include <osg/MatrixTransform>

using namespace osgGaming;
using namespace osgText;
using namespace osg;
using namespace std;

Hud::Hud()
	: Referenced(),
	_fpsEnabled(false)
{
	ref_ptr<StateSet> stateSet = new StateSet();
	stateSet->setMode(GL_BLEND, StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	stateSet->setMode(GL_LIGHTING, StateAttribute::OFF);
	stateSet->setRenderingHint(StateSet::TRANSPARENT_BIN);
	stateSet->setRenderBinDetails(10, "RenderBin");


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

void Hud::loadMarkupFromXmlResource(string resourceKey)
{
	ref_ptr<UIElement> element = UIMarkupLoader::getInstance()->loadMarkupFromXmlResource(resourceKey);

	setRootUIElement(element);
}

ref_ptr<UIElement> Hud::getRootUIElement()
{
	return _rootUIElement;
}

UserInteractionModel::List& Hud::getUserInteractionModels()
{
	return _uimList;
}

ref_ptr<UIElement> Hud::getUIElementByName(string name)
{
	UIFindElementVisitor visitor(name);

	_rootUIElement->accept(visitor);

	return visitor.getResult();
}

void Hud::updateResolution(Vec2f resolution)
{
	_resolution = resolution;

	_projection->setMatrix(Matrix::ortho2D(0.0, double(_resolution.x()) - 1.0, 1.0, double(_resolution.y()) - 1.0));

	updateUIElements();
}

void Hud::updateUIElements()
{
	Vec2f origin(0.0f, 0.0f);

	_rootUIElement->setAbsoluteOrigin(origin);
	_rootUIElement->setOrigin(origin);
	_rootUIElement->setSize(_resolution);

	UIUpdateVisitor uiUpdateVisitor;
	_rootUIElement->accept(uiUpdateVisitor);
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
		_fpsText = createTextNode("", 25.0f, ResourceManager::getInstance()->loadDefaultFont());
		_fpsText->setAlignment(TextBase::LEFT_BOTTOM);
		_fpsText->setPosition(Vec3f(10.0f, 10.0f, 0.0f));
		_fpsText->setDataVariance(Object::DYNAMIC);
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
	if (_rootUIElement == element)
	{
		return;
	}

	ref_ptr<Group> parent = _rootUIElement->getParent(0);
	parent->removeChild(_rootUIElement);

	_rootUIElement = element;

	parent->addChild(_rootUIElement);

	resetUserInteractionModel();
	_uimList.clear();

	UIMCollectorVisitor visitor;
	_rootUIElement->accept(visitor);

	_uimList = visitor.getUserInteractionModels();

	updateUIElements();
}

void Hud::registerUserInteractionModel(UserInteractionModel* model)
{
	_uimList.push_back(model);
}

bool Hud::anyUserInteractionModelHovered()
{
	for (UserInteractionModel::List::iterator it = _uimList.begin(); it != _uimList.end(); ++it)
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
	for (UserInteractionModel::List::iterator it = _uimList.begin(); it != _uimList.end(); ++it)
	{
		(*it)->setHovered(false);
	}
}
