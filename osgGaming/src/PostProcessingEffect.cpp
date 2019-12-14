#include <osgGaming/PostProcessingEffect.h>

using namespace osgGaming;

PostProcessingEffect::PostProcessingEffect()
	: Referenced()
	, m_isInitialized(false)
{

}

void PostProcessingEffect::initialize()
{
	if (!m_isInitialized)
	{
		initializeUnits();
		m_isInitialized = true;
	}
}

bool PostProcessingEffect::isInitialized() const
{
	return m_isInitialized;
}

PostProcessingEffect::InputToUniformList PostProcessingEffect::getInputToUniform()
{
	return InputToUniformList();
}