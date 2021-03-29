#pragma once

#include <QtOsgBridge/AbstractEventState.h>

#include <osgHelper/ppu/HDR.h>
#include <osgHelper/ppu/FXAA.h>
#include <osgHelper/ppu/DOF.h>

class InitialState : public QtOsgBridge::AbstractEventState
{
public:
  InitialState(osgHelper::ioc::Injector& injector);

  void initialize(QtOsgBridge::MainWindow* mainWindow) override;

  bool isLoadingState() const override;

private:
  osg::ref_ptr<osgHelper::ppu::FXAA> m_fxaa;
  osg::ref_ptr<osgHelper::ppu::DOF>  m_dof;
  osg::ref_ptr<osgHelper::ppu::HDR>  m_hdr;
};