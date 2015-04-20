#include <osgGaming/InputManager.h>

using namespace osgGaming;
using namespace osgGA;
using namespace osg;

InputManager::InputManager(ref_ptr<World> world, ref_ptr<World> worldLoading)
	: osgGA::GUIEventHandler(),
	  _world(world),
	  _worldLoading(worldLoading),
	  _resolutionWidth(0),
	  _resolutionHeight(0)
{

}

bool InputManager::handle(const GUIEventAdapter& ea, GUIActionAdapter&)
{
	switch (ea.getEventType())
	{
	case GUIEventAdapter::KEYDOWN:
	
		if (_currentState.valid())
		{
			_currentState->onKeyDownEvent(ea.getKey());
		}

		return true;
	
	case GUIEventAdapter::RESIZE:

		if (_graphicsWindow.valid())
		{
			int newWidth = _graphicsWindow->getTraits()->width;
			int newHeight= _graphicsWindow->getTraits()->height;

			if (newWidth != _resolutionWidth || newHeight != _resolutionHeight)
			{
				updateResolution(newWidth, newHeight);
			}
		}

		return true;
	}

	return false;
}

void InputManager::accept(GUIEventHandlerVisitor& v)
{
	v.visit(*this);
}

void InputManager::setGraphicsWindow(osg::ref_ptr<osgViewer::GraphicsWindow> graphicsWindow)
{
	_graphicsWindow = graphicsWindow;
}

void InputManager::setCurrentState(osg::ref_ptr<GameState> state)
{
	_currentState = state;
}

void InputManager::updateResolution(unsigned int width, unsigned int height)
{
	double ratio = (double)width / (double)height;

	_world->getCameraManipulator()->updateProjectionRatio(ratio);
	_worldLoading->getCameraManipulator()->updateProjectionRatio(ratio);

	_world->getHud()->updateResolution(width, height);
	_worldLoading->getHud()->updateResolution(width, height);

	_resolutionWidth = width;
	_resolutionHeight = height;
}
