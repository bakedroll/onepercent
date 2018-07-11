#include "MainFrameWidget.h"

#include "core/Observables.h"
#include "nodes/GlobeOverviewWorld.h"
#include "widgets/SkillsWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QLabel>

#include <osgGaming/FpsUpdateCallback.h>
#include <core/QConnectFunctor.h>

namespace onep
{
  struct MainFrameWidget::Impl
  {
    Impl(osgGaming::Injector& injector, MainFrameWidget* b)
      : base(b)
      , layoutMain(nullptr)
      , skillsWidget(new SkillsWidget(injector))
      , widgetEnabled(nullptr)
      , oDay(injector.inject<ODay>())
      , globeModel(injector.inject<GlobeModel>())
    {}

    MainFrameWidget* base;

    QVBoxLayout* layoutMain;

    SkillsWidget* skillsWidget;
    QWidget* widgetEnabled;

    ODay::Ptr oDay;
    GlobeModel::Ptr globeModel;

    osgGaming::Observer<int>::Ptr observerDay;

    void setCenterWidgetEnabled(QWidget* widget)
    {
      if (widgetEnabled)
        widgetEnabled->setVisible(false);

      if (widget)
        widget->setVisible(true);

      widgetEnabled = widget;

      layoutMain->setStretch(1, widgetEnabled ? 0 : 1);
    }

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
      layoutMain = new QVBoxLayout();
      layoutMain->setContentsMargins(0, 0, 0, 0);
      layoutMain->addWidget(frameTopBar);
      layoutMain->addStretch(1);
      layoutMain->addWidget(skillsWidget, 1);
      layoutMain->addWidget(frameBottomBar);

      skillsWidget->setVisible(false);

      base->setObjectName("MainFrameWidget");
      base->setLayout(layoutMain);

      connect(buttonDebug, SIGNAL(clicked()), base, SIGNAL(clickedButtonDebug()));

      QConnectFunctor::connect(buttonSkills, SIGNAL(clicked()), [this]()
      {
        if (widgetEnabled)
          setCenterWidgetEnabled(nullptr);
        else
          setCenterWidgetEnabled(skillsWidget);
      });

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