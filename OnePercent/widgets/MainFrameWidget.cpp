#include "MainFrameWidget.h"

#include "core/Observables.h"
#include "nodes/GlobeOverviewWorld.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QLabel>
#include <osgGaming/FpsUpdateCallback.h>

namespace onep
{
  struct MainFrameWidget::Impl
  {
    Impl(osgGaming::Injector& injector, MainFrameWidget* b)
      : base(b)
      , oDay(injector.inject<ODay>())
      , globeModel(injector.inject<GlobeModel>())
    {}

    MainFrameWidget* base;

    ODay::Ptr oDay;
    GlobeModel::Ptr globeModel;

    osgGaming::Observer<int>::Ptr observerDay;

    void setupUi()
    {
      QPushButton* buttonSkills = new QPushButton(tr("Skills"));
      buttonSkills->setObjectName("ButtonSkills");

      QPushButton* buttonDebug = new QPushButton(tr("Debug Window"));
      buttonDebug->setObjectName("ButtonDebug");

      QLabel* labelFps = new QLabel();
      labelFps->setObjectName("LabelFps");

      // Top Bar
      QHBoxLayout* layoutTopBar = new QHBoxLayout();
      layoutTopBar->setContentsMargins(0, 0, 0, 0);
      layoutTopBar->addWidget(buttonSkills);
      layoutTopBar->addStretch(1);
      layoutTopBar->addWidget(buttonDebug);
      layoutTopBar->addWidget(labelFps);

      QFrame* frameTopBar = new QFrame();
      frameTopBar->setObjectName("FrameTopBar");
      frameTopBar->setLayout(layoutTopBar);

      // Bottom Bar
      QLabel* labelDays = new QLabel();
      labelDays->setObjectName("LabelDays");

      QHBoxLayout* layoutBottomBar = new QHBoxLayout();
      layoutBottomBar->setContentsMargins(0, 0, 0, 0);
      layoutBottomBar->addWidget(labelDays);
      layoutBottomBar->addStretch(1);

      QFrame* frameBottomBar = new QFrame();
      frameBottomBar->setObjectName("FrameBottomBar");
      frameBottomBar->setLayout(layoutBottomBar);

      // Main Layout
      QVBoxLayout* layoutMain = new QVBoxLayout();
      layoutMain->setContentsMargins(0, 0, 0, 0);
      layoutMain->addWidget(frameTopBar);
      layoutMain->addStretch(1);
      layoutMain->addWidget(frameBottomBar);

      base->setObjectName("MainFrameWidget");
      base->setLayout(layoutMain);

      connect(buttonSkills, SIGNAL(clicked()), base, SIGNAL(clickedButtonSkills()));
      connect(buttonDebug, SIGNAL(clicked()), base, SIGNAL(clickedButtonDebug()));

      observerDay = oDay->connectAndNotify(osgGaming::Func<int>([labelDays](int day)
      {
        labelDays->setText(tr("Day %1").arg(day));
      }));

      osg::ref_ptr<osgGaming::FpsUpdateCallback> fpsCallback = new osgGaming::FpsUpdateCallback();
      fpsCallback->setUpdateFunc([labelFps](int fps)
      {
        labelFps->setText(QString::number(fps));
      });

      globeModel->addUpdateCallback(fpsCallback);
    }
  };

  MainFrameWidget::MainFrameWidget(osgGaming::Injector& injector)
    : VirtualOverlay()
    , m(new Impl(injector, this))
  {
    m->setupUi();
  }

  MainFrameWidget::~MainFrameWidget()
  {
  }
}