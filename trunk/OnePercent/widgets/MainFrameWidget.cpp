#include "MainFrameWidget.h"

#include "core/Macros.h"
#include "core/Observables.h"
#include "nodes/GlobeOverviewWorld.h"
#include "widgets/SkillsWidget.h"
#include "simulation/Simulation.h"

#include <osgGaming/FpsUpdateCallback.h>

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>

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
      , simulation(injector.inject<Simulation>())
    {}

    MainFrameWidget* base;

    QVBoxLayout* layoutMain;

    SkillsWidget* skillsWidget;
    QWidget*      widgetEnabled;

    ODay::Ptr       oDay;
    GlobeModel::Ptr globeModel;
    Simulation::Ptr simulation;

    osgGaming::Observer<int>::Ptr               observerDay;
    osgGaming::Observer<Simulation::State>::Ptr observerRunning;

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
      auto buttonSkills = new QPushButton(tr("Skills"));
      buttonSkills->setObjectName("ButtonSkills");

      auto buttonDebug = new QPushButton(tr("Debug Window"));
      buttonDebug->setObjectName("ButtonDebug");

      auto labelFps = new QLabel();
      labelFps->setObjectName("LabelFps");

      // simulation control
      auto buttonAutoPause = new QRadioButton();
      buttonAutoPause->setObjectName("ButtonAutoPause");
      buttonAutoPause->setIcon(QIcon("./GameData/ui/icon_auto_pause.png"));
      buttonAutoPause->setEnabled(false);
      buttonAutoPause->setChecked(true);

      auto buttonPause = new QRadioButton();
      buttonPause->setObjectName("ButtonPause");
      buttonPause->setIcon(QIcon("./GameData/ui/icon_pause.png"));
      buttonPause->setEnabled(false);
      buttonPause->setChecked(true);

      auto buttonPlay = new QRadioButton();
      buttonPlay->setObjectName("ButtonPlay");
      buttonPlay->setIcon(QIcon("./GameData/ui/icon_play.png"));
      buttonPlay->setEnabled(false);

      auto buttonFastForward = new QRadioButton();
      buttonFastForward->setObjectName("ButtonFastForward");
      buttonFastForward->setIcon(QIcon("./GameData/ui/icon_fast_forward.png"));
      buttonFastForward->setEnabled(false);

      auto simulationButtonsGroup = new QButtonGroup(base);
      simulationButtonsGroup->addButton(buttonPause, underlying(Simulation::State::Paused));
      simulationButtonsGroup->addButton(buttonPlay, underlying(Simulation::State::NormalSpeed));
      simulationButtonsGroup->addButton(buttonFastForward, underlying(Simulation::State::FastForward));

      // Top Bar
      auto layoutTopBar = new QHBoxLayout();
      layoutTopBar->setContentsMargins(0, 0, 0, 0);
      layoutTopBar->addWidget(buttonSkills);
      layoutTopBar->addStretch(1);
      layoutTopBar->addWidget(buttonDebug);
      layoutTopBar->addWidget(labelFps);

      auto frameTopBar = new QFrame();
      frameTopBar->setObjectName("FrameTopBar");
      frameTopBar->setLayout(layoutTopBar);

      // Bottom Bar
      auto labelDays = new QLabel();
      labelDays->setObjectName("LabelDays");

      auto layoutBottomBar = new QHBoxLayout();
      layoutBottomBar->setContentsMargins(0, 0, 0, 0);
      layoutBottomBar->setSpacing(0);
      layoutBottomBar->addWidget(labelDays);
      layoutBottomBar->addStretch(1);
      layoutBottomBar->addWidget(buttonAutoPause);
      layoutBottomBar->addWidget(buttonPause);
      layoutBottomBar->addWidget(buttonPlay);
      layoutBottomBar->addWidget(buttonFastForward);

      auto frameBottomBar = new QFrame();
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

      connect(buttonDebug,&QPushButton::clicked, base, &MainFrameWidget::clickedButtonDebug);

      observerRunning = simulation->getOState()->connect(
        osgGaming::Func<Simulation::State>([this, buttonAutoPause, buttonPause, buttonPlay, buttonFastForward, simulationButtonsGroup](Simulation::State state)
        {
          QSignalBlocker blocker(simulationButtonsGroup);

          if (state != Simulation::State::Paused)
          {
            buttonAutoPause->setEnabled(true);
            buttonPause->setEnabled(true);
            buttonPlay->setEnabled(true);
            buttonFastForward->setEnabled(true);

            if (state == Simulation::State::NormalSpeed)
            {
              buttonPlay->setChecked(true);
            }
            else
            {
              buttonFastForward->setChecked(true);
            }
          }
          else
          {
            buttonPause->setChecked(true);
          }
        }));

      connect(simulationButtonsGroup, QOverload<int, bool>::of(&QButtonGroup::buttonToggled), [this](int id, bool checked)
      {
        if (!checked)
        {
          return;
        }

        simulation->setState(static_cast<Simulation::State>(id));
      });

      connect(buttonSkills, &QPushButton::clicked, [this]()
      {
        if (widgetEnabled)
        {
          setCenterWidgetEnabled(nullptr);
          return;
        }

        setCenterWidgetEnabled(skillsWidget);
      });

      observerDay = oDay->connectAndNotify(osgGaming::Func<int>([this, labelDays, buttonAutoPause](int day)
      {
        labelDays->setText(tr("Day %1").arg(day));

        if (buttonAutoPause->isChecked() && (day != 0) && (day % 100 == 0))
        {
          simulation->stop();
        }
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

  MainFrameWidget::~MainFrameWidget() = default;
}
