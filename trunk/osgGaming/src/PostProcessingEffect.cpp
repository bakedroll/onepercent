#include <osgGaming/PostProcessingEffect.h>

using namespace osgGaming;

PostProcessingEffect::PostProcessingEffect()
	: Referenced(),
	  _initialized(false)
{

}

void PostProcessingEffect::initialize()
{
	if (!_initialized)
	{
		initializeUnits();
		_initialized = true;
	}
}

PostProcessingEffect::InputToUniformList PostProcessingEffect::getInputToUniform()
{
	InputToUniformList list;
	return list;
}