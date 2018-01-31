#pragma once

#include "nodes/BoundariesMesh.h"
#include "nodes/CountryOverlay.h"
#include "simulation/Simulation.h"

#include <QDialog>
#include <memory>

namespace onep
{
  class DebugWindow : public QDialog
  {
    Q_OBJECT

  public:
    DebugWindow(
      osg::ref_ptr<CountryOverlay> countryOverlay,
      osg::ref_ptr<BoundariesMesh> boundariesMesh,
      osg::ref_ptr<Simulation> simulation, 
      QWidget* parent = nullptr);
    ~DebugWindow();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}