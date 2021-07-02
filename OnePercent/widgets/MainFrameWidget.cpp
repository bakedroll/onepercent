#include "MainFrameWidget.h"

#include "core/Observables.h"
#include "nodes/GlobeOverviewWorld.h"
#include "nodes/CountryOverlay.h"
#include "widgets/SkillsWidget.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaSimulationStateTable.h"
#include "simulation/Simulation.h"
#include "simulation/ModelContainer.h"

#include <osgHelper/FpsUpdateCallback.h>

#include <luaHelper/LuaValueDef.h>
#include <luaHelper/LuaArrayTable.h>

#include <QPointer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QFormLayout>

#include <osgHelper/Helper.h>

namespace onep
{
  struct MainFrameWidget::Impl
  {
    Impl(osgHelper::ioc::Injector& injector, MainFrameWidget* b)
      : base(b)
      , layoutMain(nullptr)
      , frameLeftMenu(nullptr)
      , skillsWidget(new SkillsWidget(injector))
      , widgetEnabled(nullptr)
      , oDay(injector.inject<ODay>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
      , globeModel(injector.inject<GlobeModel>())
      , simulation(injector.inject<Simulation>())
      , countryOverlay(injector.inject<CountryOverlay>())
      , modelContainer(injector.inject<ModelContainer>())
    {}

    MainFrameWidget* base;

    QVBoxLayout* layoutMain;

    QFrame*       frameLeftMenu;
    SkillsWidget* skillsWidget;
    QWidget*      widgetEnabled;

    ODay::Ptr            oDay;
    ONumSkillPoints::Ptr oNumSkillPoints;
    GlobeModel::Ptr      globeModel;
    Simulation::Ptr      simulation;
    CountryOverlay::Ptr  countryOverlay;
    ModelContainer::Ptr  modelContainer;

    osgHelper::Observer<int>::Ptr               observerDay;
    osgHelper::Observer<int>::Ptr               observerSkillPoints;
    osgHelper::Observer<int>::Ptr               observerCurrentCountry;
    osgHelper::Observer<Simulation::State>::Ptr observerRunning;

    osg::ref_ptr<osgHelper::FpsUpdateCallback> fpsCallback;

    std::map<std::string, QPointer<QLabel>> valueLabelMap;

    void setCenterWidgetEnabled(QWidget* widget)
    {
      if (widgetEnabled)
        widgetEnabled->setVisible(false);

      if (widget)
        widget->setVisible(true);

      widgetEnabled = widget;
      Q_EMIT base->toggledWidgetEnabled(widgetEnabled != nullptr);

      layoutMain->setStretch(1, widgetEnabled ? 0 : 1);
    }

    void setupUi()
    {
      auto buttonSkills = new QPushButton(tr("Skills"));
      buttonSkills->setObjectName("ButtonSkills");

      auto labelSkillPoints = new QLabel();
      labelSkillPoints->setObjectName("LabelSkillPoints");

      auto buttonDebug = new QPushButton(tr("Debug Window"));
      buttonDebug->setObjectName("ButtonDebug");

      auto labelFps = new QLabel();
      labelFps->setObjectName("LabelFps");

      // simulation control
      auto buttonAutoPause = new QRadioButton();
      buttonAutoPause->setObjectName("ButtonAutoPause");
      buttonAutoPause->setIcon(QIcon("./GameData/ui/icon_auto_pause.png"));
      buttonAutoPause->setEnabled(false);
      buttonAutoPause->setChecked(simulation->isAutoPauseEnabled());

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
      simulationButtonsGroup->addButton(buttonPause, osgHelper::underlying(Simulation::State::Paused));
      simulationButtonsGroup->addButton(buttonPlay, osgHelper::underlying(Simulation::State::NormalSpeed));
      simulationButtonsGroup->addButton(buttonFastForward, osgHelper::underlying(Simulation::State::FastForward));

      std::map<QAbstractButton*, int> buttonIdMap;
      buttonIdMap[buttonPause]       = osgHelper::underlying(Simulation::State::Paused);
      buttonIdMap[buttonPlay]        = osgHelper::underlying(Simulation::State::NormalSpeed);
      buttonIdMap[buttonFastForward] = osgHelper::underlying(Simulation::State::FastForward);

      // Values Form
      auto layoutValues = new QFormLayout();
      layoutValues->setContentsMargins(0, 0, 0, 0);

      modelContainer->accessModel([=](const LuaModel::Ptr& model)
      {
        auto values = model->getValuesDefTable();
        values->iterateMappedObjects<luaHelper::LuaValueDef>([=](luaHelper::LuaValueDef::Ptr& def)
        {
          if (!def->getIsVisible())
          {
            return;
          }

          const auto name = QString::fromStdString(def->getName());

          auto labelName = new QLabel(name);
          labelName->setObjectName("LabelValue");

          auto labelValue = new QLabel("");
          labelValue->setObjectName("LabelValue");
          labelValue->setAlignment(Qt::AlignRight);
          labelValue->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

          layoutValues->addRow(labelName, labelValue);

          valueLabelMap[def->getName()] = labelValue;
        });
      });

      // Left Menu
      auto layoutLeftMenu = new QVBoxLayout();
      layoutLeftMenu->setContentsMargins(0, 0, 0, 0);
      layoutLeftMenu->addLayout(layoutValues);

      frameLeftMenu = new QFrame();
      frameLeftMenu->setContentsMargins(0, 0, 0, 0);
      frameLeftMenu->setObjectName("FrameLeftMenu");
      frameLeftMenu->setLayout(layoutLeftMenu);

      // Top Bar
      auto layoutTopBar = new QHBoxLayout();
      layoutTopBar->setContentsMargins(0, 0, 0, 0);
      layoutTopBar->addWidget(buttonSkills);
      layoutTopBar->addWidget(labelSkillPoints);
      layoutTopBar->addStretch(1);
      layoutTopBar->addWidget(buttonDebug);
      layoutTopBar->addWidget(labelFps);

      auto frameTopBar = new QFrame();
      frameTopBar->setObjectName("FrameTopBar");
      frameTopBar->setLayout(layoutTopBar);

      // Bottom Bar
      QPointer<QLabel> labelDays = new QLabel();
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

      // Central Layout
      auto layoutCentral = new QHBoxLayout();
      layoutCentral->setContentsMargins(0, 0, 0, 0);
      layoutCentral->addWidget(frameLeftMenu);
      layoutCentral->addWidget(skillsWidget, 1);
      layoutCentral->addStretch(1);

      // Main Layout
      layoutMain = new QVBoxLayout();
      layoutMain->setContentsMargins(0, 0, 0, 0);
      layoutMain->addWidget(frameTopBar);
      layoutMain->addLayout(layoutCentral, 1);
      layoutMain->addWidget(frameBottomBar);

      skillsWidget->setVisible(false);

      base->setObjectName("MainFrameWidget");
      base->setLayout(layoutMain);

      connect(buttonDebug,&QPushButton::clicked, base, &MainFrameWidget::clickedButtonDebug);

      observerRunning = simulation->getOState()->connect(
        [buttonAutoPause, buttonPause, buttonPlay, buttonFastForward, simulationButtonsGroup](Simulation::State state)
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
        });

      connect(simulationButtonsGroup, QOverload<QAbstractButton*, bool>::of(&QButtonGroup::buttonToggled),
              [this, buttonIdMap](QAbstractButton* button, bool checked)
      {
        if (!checked)
        {
          return;
        }

        simulation->setState(static_cast<Simulation::State>(buttonIdMap.find(button)->second));
      });

      connect(buttonAutoPause, &QRadioButton::toggled, [this](bool checked)
      {
        simulation->setAutoPause(checked);
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

      observerDay = oDay->connectAndNotify([this, labelDays](int day)
      {
        if (!labelDays)
        {
          return;
        }

        labelDays->setText(tr("Day %1").arg(day));

        const auto selectedCountryId = countryOverlay->getSelectedCountryId();
        if (selectedCountryId == 0)
        {
          return;
        }

        modelContainer->accessModel([this, selectedCountryId](const LuaModel::Ptr& model)
        {
          const auto& valuesMap = model->getSimulationStateTable()->getCountryState(selectedCountryId)->getValuesTable()->getMap();
          for (const auto& label : valueLabelMap)
          {
            const auto it = valuesMap.find(label.first);
            assert_continue(it != valuesMap.end())

            label.second->setText(QString::number(it->second));
          }
        });
      });

      observerSkillPoints = oNumSkillPoints->connectAndNotify([labelSkillPoints](int points)
      {
        labelSkillPoints->setText(tr("Skillpoints: %1").arg(points));
      });

      observerCurrentCountry = countryOverlay->getOSelectedCountryId()->connectAndNotify([this](int id)
      {
        frameLeftMenu->setVisible(id > 0);
      });

      fpsCallback = new osgHelper::FpsUpdateCallback();
      fpsCallback->setUpdateFunc([labelFps](int fps)
      {
        labelFps->setText(QString::number(fps));
      });

      globeModel->addUpdateCallback(fpsCallback);
    }
  };

  MainFrameWidget::MainFrameWidget(osgHelper::ioc::Injector& injector)
    : QWidget()
    , m(new Impl(injector, this))
  {
    m->setupUi();
  }

  MainFrameWidget::~MainFrameWidget()
  {
    m->globeModel->removeUpdateCallback(m->fpsCallback);
  }
}
