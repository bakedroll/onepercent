#include <osgGaming/FpsTextCallback.h>

#include <osgText/Text>

using namespace osg;
using namespace osgGaming;
using namespace osgText;

FpsTextCallback::FpsTextCallback()
	: osg::Drawable::UpdateCallback(),
	  _framesCount(0),
	  _lastSimulationTime(0.0)
{

}

void FpsTextCallback::update(osg::NodeVisitor* nv, osg::Drawable* drawable)
{
	double time = nv->getFrameStamp()->getSimulationTime();

	if (_lastSimulationTime != 0.0)
	{
		if (time - _lastSimulationTime >= 1.0)
		{
			ref_ptr<Text> fpsText = static_cast<Text*>(drawable);
			fpsText->setText(std::to_string(_framesCount));

			_framesCount = 0;
			_lastSimulationTime = time;
		}
		else
		{
			_framesCount++;
		}
	}
	else
	{
		_lastSimulationTime = time;
	}

}