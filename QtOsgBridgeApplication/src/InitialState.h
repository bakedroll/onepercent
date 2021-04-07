#pragma once

#include <QtOsgBridge/EventProcessingState.h>
#include <QtOsgBridge/VirtualOverlay.h>

#include <osgHelper/ppu/HDR.h>
#include <osgHelper/ppu/FXAA.h>
#include <osgHelper/ppu/DOF.h>

#include <QPointer>

class State1 : public QtOsgBridge::EventProcessingState
{
public:
  State1(osgHelper::ioc::Injector& injector);
  ~State1() override;

  void onInitialize(QPointer<QtOsgBridge::MainWindow> mainWindow) override;
  void onExit() override;

private:
  QPointer<QtOsgBridge::VirtualOverlay>        m_overlay;
  osg::ref_ptr<QtOsgBridge::OverlayCompositor> m_overlayCompositor;

};

class State2 : public QtOsgBridge::EventProcessingState
{
public:
  State2(osgHelper::ioc::Injector& injector);
  ~State2() override;

  void onInitialize(QPointer<QtOsgBridge::MainWindow> mainWindow) override;
  void onExit() override;

private:
  QPointer<QtOsgBridge::VirtualOverlay>        m_overlay;
  osg::ref_ptr<QtOsgBridge::OverlayCompositor> m_overlayCompositor;

};

class InitialState : public QtOsgBridge::EventProcessingState
{
  Q_OBJECT

public:
  InitialState(osgHelper::ioc::Injector& injector);
  ~InitialState() override;

  void onInitialize(QPointer<QtOsgBridge::MainWindow> mainWindow) override;
  void onExit() override;
  void onUpdate(const SimulationData& data) override;

protected:
  bool onMouseEvent(QMouseEvent* event) override;
  bool onKeyEvent(QKeyEvent* event) override;

private:
  osg::ref_ptr<osgHelper::ppu::FXAA> m_fxaa;
  osg::ref_ptr<osgHelper::ppu::DOF>  m_dof;
  osg::ref_ptr<osgHelper::ppu::HDR>  m_hdr;

  float  m_rotation;
  double m_position;

  QPointer<QtOsgBridge::MainWindow>     m_mainWindow;
  QPointer<QtOsgBridge::VirtualOverlay> m_overlay;

  osg::ref_ptr<osg::PositionAttitudeTransform> m_boxTransform;

  void updateTransformation();

};