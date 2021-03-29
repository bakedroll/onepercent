#include "QtOsgBridgeApplication.h"
#include "InitialState.h"

QtOsgBridgeApplication::QtOsgBridgeApplication(int& argc, char** argv)
  : QtGameApplication(argc, argv)
{
}

void QtOsgBridgeApplication::registerComponents(osgHelper::ioc::InjectionContainer& container)
{
  registerEssentialComponents();

  container.registerType<InitialState>();
  container.registerType<osgHelper::ppu::DOF>();
  container.registerType<osgHelper::ppu::FXAA>();
  container.registerType<osgHelper::ppu::HDR>();
}
