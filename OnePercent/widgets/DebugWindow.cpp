#include "DebugWindow.h"

#include "core/Observables.h"
#include "scripting/LuaCountry.h"
#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaSkillsTable.h"
#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaControl.h"
#include "simulation/Simulation.h"
#include "simulation/UpdateThread.h"
#include "simulation/ModelContainer.h"

#include <osgHelper/Macros.h>

#include <luaHelper/LuaStateManager.h>

#include <QVBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>
#include <QLabel>
#include <QKeyEvent>
#include <QFileDialog>
#include <QDoubleSpinBox>
#include <QProgressBar>
#include <QScrollArea>
#include <QPainter>

#include <QtOsgBridge/Multithreading.h>
#include <QtOsgBridge/Macros.h>

#include <chrono>
#include <nodes/CountryOverlay.h>
#include <nodes/BoundariesMesh.h>

namespace onep
{
  ConsoleEdit::ConsoleEdit(QWidget* parent)
    : QLineEdit(parent)
    , m_pos(-1)
  {
    setFont(QFont("Lucida Console", 12));

    connect(this, &QLineEdit::returnPressed, [&]()
    {
      QString command = text();
      m_latestCommands.push_back(command);
      emit commandEntered(command);
      setText(QString());
      m_pos = -1;
    });
  }

  void ConsoleEdit::keyPressEvent(QKeyEvent* event)
  {
    if (event->key() == Qt::Key_Up && m_latestCommands.size() > 0)
    {
      if (m_pos == -1)
      {
        m_pos = m_latestCommands.size() - 1;
        setText(m_latestCommands[m_pos]);
      }
      else if (m_pos > 0)
      {
        m_pos--;
        setText(m_latestCommands[m_pos]);
      }
    }
    else if (event->key() == Qt::Key_Down && m_latestCommands.size() > 0)
    {
      if (m_pos < int(m_latestCommands.size()-1))
      {
        m_pos++;
        setText(m_latestCommands[m_pos]);
      }
    }
    else if (event->key() == Qt::Key_Escape)
    {
      setText(QString());
    }

    QLineEdit::keyPressEvent(event);
  }

  struct DebugWindow::Impl
  {
    Impl(osgHelper::ioc::Injector& injector, DebugWindow* b)
      : base(b)
      , lua(injector.inject<luaHelper::LuaStateManager>())
      , countryOverlay(injector.inject<CountryOverlay>())
      , boundariesMesh(injector.inject<BoundariesMesh>())
      , simulation(injector.inject<Simulation>())
      , modelContainer(injector.inject<ModelContainer>())
      , luaControl(injector.inject<LuaControl>())
      , oDay(injector.inject<ODay>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
      , toggleCountryButton(nullptr)
      , scrollAreaStats(nullptr)
      , widgetStats(nullptr)
      , layoutStats(nullptr)
      , radioNoOverlay(nullptr)
      , labelSkillpoints(nullptr)
      , labelCountry(nullptr)
      , buttonStartStop(nullptr)
      , checkBoxEnableGraph(nullptr)
      , bWireframe(false)
      , borderThickness(0.1f)
    {
    }

    DebugWindow* base;

    osg::ref_ptr<luaHelper::LuaStateManager> lua;

    osg::ref_ptr<CountryOverlay> countryOverlay;
    osg::ref_ptr<BoundariesMesh> boundariesMesh;
    osg::ref_ptr<Simulation> simulation;
    osg::ref_ptr<ModelContainer> modelContainer;
    LuaControl::Ptr luaControl;

    ODay::Ptr oDay;
    ONumSkillPoints::Ptr oNumSkillPoints;

    QPushButton*                   toggleCountryButton;
    BoundariesData::CountryBorders selectedCountries;

    osgHelper::Observer<int>::Ptr               notifySelectedCountry;
    osgHelper::Observer<int>::Ptr               notifyDay;
    osgHelper::Observer<int>::Ptr               notifySkillpoints;
    osgHelper::Observer<Simulation::State>::Ptr notifyRunningValues;
    osgHelper::Observer<Simulation::State>::Ptr notifyRunningButton;

    QScrollArea* scrollAreaStats;
    QWidget* widgetStats;
    QVBoxLayout* layoutStats;
    QRadioButton* radioNoOverlay;
    QLabel* labelSkillpoints;
    QLabel* labelCountry;
    ConsoleEdit* luaConsoleEdit;
    QPushButton* buttonStartStop;
    QCheckBox* checkBoxEnableGraph;

    std::vector<osgHelper::Observer<int>::Ptr>         selectedCountryIdObservers;
    std::vector<osgHelper::Observer<bool>::Ptr>        skillBranchActivatedObservers;
    std::vector<osgHelper::Observer<bool>::Ptr>        skillActivatedObservers;
    std::vector<osgHelper::Observer<std::string>::Ptr> currentOverlayBranchNameObservers;

    struct ValueWidget
    {
      ValueWidget() : pixGraph(100, 36) {}

      QLineEdit* edit;
      QPushButton* buttonSet;
      QProgressBar* progressBar;
      QLabel* labelGraph;
      QPixmap pixGraph;
      std::list<float> latestValues;
    };

    typedef std::map<std::string, ValueWidget> ValueWidgets;

    ValueWidgets valueWidgets;

    bool bWireframe;
    float borderThickness;

    void updateBoundaries()
    {
      auto start_time = std::chrono::high_resolution_clock::now();
      boundariesMesh->makeCountryBoundaries(selectedCountries, osg::Vec3f(1.0f, 0.5, 0.1f), borderThickness, bWireframe);
      auto duration = std::chrono::high_resolution_clock::now() - start_time;
      long d = std::chrono::duration_cast<std::chrono::milliseconds> (duration).count();

      OSGH_QLOG_DEBUG(QString("Took %1 ms").arg(d));
    }

    void toggleCountry()
    {
      int id = countryOverlay->getSelectedCountryId();

      if (id == 0)
        return;

      auto& borders = countryOverlay->getCountryPresenter(id)->getNeighborBorders();

      const auto it = selectedCountries.find(id);
      if (it == selectedCountries.end())
        selectedCountries[id] = borders;
      else
        selectedCountries.erase(it);

      updateBoundaries();
    }

    ValueWidget createValueWidgets(const QString& labelText, std::function<void(float, LuaCountryState::Ptr)> setFunc)
    {
      ValueWidget widget;

      QLabel* label = new QLabel(labelText);
      label->setMinimumWidth(150);

      widget.edit = new QLineEdit();
      widget.edit->setMaximumWidth(90);

      widget.buttonSet = new QPushButton("Set");
      widget.buttonSet->setMaximumWidth(50);
      widget.buttonSet->setEnabled(false);
      widget.buttonSet->setFocusPolicy(Qt::NoFocus);

      widget.progressBar = new QProgressBar();
      widget.progressBar->setOrientation(Qt::Vertical);
      widget.progressBar->setRange(0, 100);
      widget.progressBar->setFixedSize(20, 36);

      widget.labelGraph = new QLabel();
      widget.labelGraph->setPixmap(widget.pixGraph);

      QPushButton* button = widget.buttonSet;
      connect(widget.edit, &QLineEdit::textEdited, [button]()
      {
        button->setEnabled(true);
      });

      QLineEdit* edit = widget.edit;
      connect(widget.buttonSet, &QPushButton::clicked, [=]()
      {
        int id = countryOverlay->getSelectedCountryId();
        if (id == 0) return;

        modelContainer->accessModel([=](const LuaModel::Ptr& model)
        {
          LuaCountryState::Ptr cstate = model->getSimulationStateTable()->getCountryState(id);

          bool ok;
          float value = edit->text().toFloat(&ok);
          setFunc(value, cstate);
          edit->setText(QString::number(value));
          button->setEnabled(false);
        });
      });

      QHBoxLayout* layout = new QHBoxLayout();
      layout->setContentsMargins(0, 0, 0, 0);
      layout->setSpacing(2);
      layout->addWidget(label);
      layout->addStretch(1);
      layout->addWidget(widget.edit);
      layout->addWidget(widget.buttonSet);
      layout->addWidget(widget.progressBar);
      layout->addWidget(widget.labelGraph);

      layoutStats->addLayout(layout);

      return widget;
    }

    void clearValueGraphs()
    {
      for (ValueWidgets::iterator it = valueWidgets.begin(); it != valueWidgets.end(); ++it)
      {
        it->second.latestValues.clear();
        it->second.pixGraph.fill(QColor(255, 255, 255));
        it->second.labelGraph->setPixmap(it->second.pixGraph);
      }
    }

    void updateValueGraph(ValueWidget& widget, float value)
    {
      int w = widget.pixGraph.width();
      int h = widget.pixGraph.height();

      widget.latestValues.push_front(osg::clampBetween<float>(value, 0.0f, 1.0f));
      if (int(widget.latestValues.size()) > w)
        widget.latestValues.pop_back();

      QPainter painter(&widget.pixGraph);
      painter.setPen(QColor(255, 0, 0));

      widget.pixGraph.fill(QColor(255, 255, 255));
      float lastValue = 0.0f;
      int pos = 0;
      for (std::list<float>::iterator it = widget.latestValues.begin(); it != widget.latestValues.end(); ++it)
      {
        float val = *it;
        pos++;

        if (it != widget.latestValues.begin())
          painter.drawLine(w - pos, (h - 1) - lastValue * (h - 1), w - pos + 1, (h - 1) - val * (h - 1));

        lastValue = val;
      }

      widget.labelGraph->setPixmap(widget.pixGraph);
    }

    void updateCountryInfo()
    {
      updateCountryInfo(countryOverlay->getSelectedCountryId());
    }

    void addValueWidget(const luaHelper::LuaValueGroupTable::Ptr& values, const std::string& prefix = "")
    {
      for (const auto& value : values->getMap())
      {
        const auto name = value.first;
        const auto displayName = prefix + name;
        ValueWidget widget = createValueWidgets(QString("%1").arg(displayName.c_str()), [=](float value, LuaCountryState::Ptr cstate)
        {
          values->setValue(name, value);
        });

        valueWidgets[displayName] = widget;
      }
    }

    void initValueWidget(const luaHelper::LuaValueGroupTable::Ptr& values, const std::string& prefix = "")
    {
      for (const auto& value : values->getMap())
      {
        const auto name        = value.first;
        const auto displayName = prefix + name;
        auto&      widget      = valueWidgets[displayName];

        widget.edit->setText(QString::number(value.second));
        widget.progressBar->setValue(osg::clampBetween<int>(int(value.second * 100.0f), 0, 100));

        if (checkBoxEnableGraph->isChecked())
        {
          updateValueGraph(widget, value.second);
        }
      }
    }

    void updateCountryInfo(int selectedId)
    {
      // cancel update if DebugWindow is not shown
      if (!base->isVisible())
        return;

      widgetStats->setVisible(selectedId > 0);
      if (selectedId == 0)
        return;

      modelContainer->accessModel([=](const LuaModel::Ptr& model)
      {
        auto& countryStates = model->getSimulationStateTable()->getCountryStates();
        if (countryStates.count(selectedId) == 0)
        {
          return;
        }

        auto country = model->getCountriesTable()->getCountryById(selectedId);
        assert_return(country);

        auto countryState = countryStates[selectedId];

        auto        values = countryState->getValuesTable();
        const auto& groups = countryState->getValuesTable()->getAllGroups();

        if (layoutStats == nullptr)
        {
          layoutStats = new QVBoxLayout();
          layoutStats->setContentsMargins(10, 10, 10, 10);
          layoutStats->setSpacing(0);
          widgetStats->setLayout(layoutStats);

          scrollAreaStats->setWidget(widgetStats);

          labelCountry = new QLabel();

          layoutStats->addWidget(labelCountry);

          addValueWidget(values);

          for (const auto& group : groups)
          {
              addValueWidget(group.second, group.first + ".");
          }

          notifyRunningValues = simulation->getOState()->connectAndNotify([this](Simulation::State state)
          {
            auto paused = (state == Simulation::State::Paused);

            for (ValueWidgets::iterator it = valueWidgets.begin(); it != valueWidgets.end(); ++it)
              it->second.edit->setEnabled(paused);
          });
        }

        labelCountry->setText(QString("%1 (%2)").arg(QString::fromLocal8Bit(country->getName().c_str())).arg(country->getId()));


        initValueWidget(values);

        for (const auto& group : groups)
        {
          initValueWidget(group.second, group.first + ".");
        }
      });
    }

    void setupUi()
    {
      auto generateLuaDocButton = new QPushButton("Generate LuaDoc");
      generateLuaDocButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);

#ifdef LUADOC_ENABLED
      connect(generateLuaDocButton, &QPushButton::clicked, [this]()
      {
        auto dir = QFileDialog::getExistingDirectory(base, "Select output directory", QDir::current().path());
        if (dir.isEmpty())
        {
          return;
        }

        luadoc::DocsGenerator::instance().generate("OnePercent", dir, LUADOC_DESCRIPTIONS_FILE);
      });
#else
      generateLuaDocButton->setEnabled(false);
      generateLuaDocButton->setToolTip("LuaDoc not enabled for this build");
#endif
      


      toggleCountryButton = new QPushButton("Toggle selected country");
      toggleCountryButton->setFocusPolicy(Qt::NoFocus);

      QCheckBox* checkBoxWireframe = new QCheckBox("Wireframe Borders");
      checkBoxWireframe->setChecked(bWireframe);
      checkBoxWireframe->setFocusPolicy(Qt::NoFocus);

      QDoubleSpinBox* spinboxThickness = new QDoubleSpinBox();
      spinboxThickness->setValue(borderThickness);
      spinboxThickness->setRange(0.01, 999.0);
      spinboxThickness->setSingleStep(0.01);

      connect(toggleCountryButton, &QPushButton::clicked, [this]()
      {
        toggleCountry();
      });

      connect(checkBoxWireframe, &QCheckBox::toggled, [this](bool checked)
      {
        bWireframe = checked;
        updateBoundaries();
      });

      connect(spinboxThickness, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value)
      {
        borderThickness = float(value);
        updateBoundaries();
      });

      auto branchesLayout = new QGridLayout();
      auto radioGroup     = new QButtonGroup(base);

      radioNoOverlay = new QRadioButton(QObject::tr("No Overlay"));
      radioNoOverlay->setChecked(true);
      radioGroup->addButton(radioNoOverlay);

      connect(radioNoOverlay, &QRadioButton::clicked, [=]()
      {
        countryOverlay->setCurrentOverlayBranchName("");
      });

      branchesLayout->addWidget(radioNoOverlay, 0, 1);

      const auto& branches = modelContainer->getModel()->getBranchesTable()->getBranches();
      auto        index    = 0;

      for (const auto& branch : branches)
      {
        const auto& name = branch.second->getName();

        QCheckBox* checkBox = new QCheckBox(QString::fromStdString(name));
        QRadioButton* radioButton = new QRadioButton(QObject::tr("Overlay"));
        radioGroup->addButton(radioButton);

        branchesLayout->addWidget(checkBox, 1 + index, 0);
        branchesLayout->addWidget(radioButton, 1 + index, 1);

        connect(checkBox, &QCheckBox::clicked, [=](bool checked)
        {
          int cid = countryOverlay->getSelectedCountryId();
          if (cid == 0)
          {
            return;
          }

          if (checked)
          {
            modelContainer->accessModel([this, &branch](const LuaModel::Ptr&)
            {
              simulation->addSkillPoints(branch.second->getCost());
            });
          }

          simulation->switchSkillBranchState(cid, branch.second,
                                             checked ? Simulation::SkillBranchState::PURCHASED
                                                     : Simulation::SkillBranchState::RESIGNED);
        });

        connect(radioButton, &QRadioButton::clicked, [=](bool)
        {
          countryOverlay->setCurrentOverlayBranchName(name);
        });

        currentOverlayBranchNameObservers.push_back(countryOverlay->getOCurrentOverlayBranchName()->connectAndNotify(
          [=](std::string currentName)
        {
          if (currentName.empty())
          {
            QSignalBlocker blocker(radioNoOverlay);
            radioNoOverlay->setChecked(true);
            return;
          }

          if (currentName == name)
          {
            QSignalBlocker blocker(radioButton);
            radioButton->setChecked(true);
          }
        }));

        selectedCountryIdObservers.push_back(countryOverlay->getOSelectedCountryId()->connectAndNotify(
          [=](int selected)
        {
          QSignalBlocker blocker(checkBox);

          if (selected > 0)
          {
            modelContainer->accessModel([=](const LuaModel::Ptr& model)
            {
              checkBox->setChecked(
                model->getSimulationStateTable()->getCountryState(selected)->getBranchesActivatedTable()->getBranchActivated(name));
            });
          }
          else
          {
            checkBox->setChecked(false);
          }

          checkBox->setEnabled(selected > 0);
        }));

        modelContainer->accessModel([this, name, checkBox](const LuaModel::Ptr& model)
        {
          for(auto& it : model->getSimulationStateTable()->getCountryStates())
          {
            int cid = it.first;

            skillBranchActivatedObservers.push_back(it.second->getBranchesActivatedTable()->getOBranchActivated(name)->connect([=](bool activated)
            {
              QtOsgBridge::Multithreading::executeInUiAsync([=]()
              {
                if (cid == countryOverlay->getSelectedCountryId())
                {
                  QSignalBlocker blocker(checkBox);
                  checkBox->setChecked(activated);
                }
              });
            }));
          }
        });

        index++;
      }

      luaConsoleEdit = new ConsoleEdit();
      luaConsoleEdit->setFocusPolicy(Qt::FocusPolicy::StrongFocus);

      buttonStartStop = new QPushButton();
      buttonStartStop->setFixedWidth(100);
      buttonStartStop->setFocusPolicy(Qt::NoFocus);

      connect(luaConsoleEdit, &ConsoleEdit::commandEntered, base, &DebugWindow::onCommandEntered);

      connect(buttonStartStop, &QPushButton::clicked, [&]()
      {
        if (simulation->isRunning())
          simulation->stop();
        else
          simulation->start();
      });

      notifyRunningButton = simulation->getOState()->connectAndNotify([&](Simulation::State state)
      {
        if (state != Simulation::State::Paused)
          buttonStartStop->setText("Stop simulation");
        else
          buttonStartStop->setText("Start simulation");
      });

      widgetStats = new QWidget();
      labelSkillpoints = new QLabel(QString());
      labelSkillpoints->setObjectName("LabelSkillPoints");

      scrollAreaStats = new QScrollArea();
      scrollAreaStats->setWidgetResizable(true);
      scrollAreaStats->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      scrollAreaStats->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      scrollAreaStats->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

      QVBoxLayout* rightLayout = new QVBoxLayout();

      // Skills
      for (const auto& branch : branches)
      {
        const auto& name = branch.second->getName();

        // Skills
        const auto& skills = branch.second->getSkillsTable()->getSkills();

        QLabel* label = new QLabel(QString::fromStdString(name));
        rightLayout->addWidget(label);

        for (const auto& skill : skills)
        {
          QCheckBox* checkBox = new QCheckBox(QString::fromStdString(skill.second->getDisplayName()));
          checkBox->setChecked(skill.second->getIsActivated());
          rightLayout->addWidget(checkBox);

          auto skillPtr = skill.second;

          connect(checkBox, &QCheckBox::clicked, [this, skillPtr](bool checked)
          {
            modelContainer->accessModel([this, skillPtr, checked](const LuaModel::Ptr&)
            {
              skillPtr->setIsActivated(checked);
            });
          });

          skillActivatedObservers.push_back(skillPtr->getObActivated()->connect([=](bool activated)
          {
            QSignalBlocker blocker(checkBox);
            checkBox->setChecked(activated);
          }));
        }
      }

      rightLayout->addStretch(1);

      QWidget* rightWidget = new QWidget();
      rightWidget->setContentsMargins(0, 0, 0, 0);
      rightWidget->setObjectName("WidgetRightPanel");
      rightWidget->setLayout(rightLayout);

      QPushButton* loadStateButton = new QPushButton("Load state");
      loadStateButton->setFocusPolicy(Qt::NoFocus);

      QPushButton* saveStateButton = new QPushButton("Save state");
      saveStateButton->setFocusPolicy(Qt::NoFocus);

      checkBoxEnableGraph = new QCheckBox(tr("Enable graph"));
      checkBoxEnableGraph->setChecked(true);

      connect(checkBoxEnableGraph, &QCheckBox::toggled, [this](bool checked)
      {
        if (!checked)
        {
          clearValueGraphs();
        }
      });

      connect(loadStateButton, &QPushButton::clicked, [this]()
      {
        QString filename = QFileDialog::getOpenFileName(base, "Load state", QDir::currentPath(), "State files (*.ste)");
        if (!filename.isEmpty())
        {
          simulation->loadState(filename.toStdString());
          updateCountryInfo();
        }
      });

      connect(saveStateButton, &QPushButton::clicked, [this]()
      {
        QString filename = QFileDialog::getSaveFileName(base, "Save state", QDir::currentPath(), "State files (*.ste)");
        if (!filename.isEmpty())
          simulation->saveState(filename.toStdString());
      });

      QHBoxLayout* saveLoadStateLayout = new QHBoxLayout();
      saveLoadStateLayout->setContentsMargins(0, 0, 0, 0);
      saveLoadStateLayout->addWidget(loadStateButton);
      saveLoadStateLayout->addWidget(saveStateButton);

      QVBoxLayout* miscControlsLayout = new QVBoxLayout();
      miscControlsLayout->addWidget(generateLuaDocButton);
      miscControlsLayout->addWidget(toggleCountryButton);
      miscControlsLayout->addWidget(checkBoxWireframe);
      miscControlsLayout->addWidget(spinboxThickness);
      miscControlsLayout->addWidget(checkBoxEnableGraph);
      miscControlsLayout->addLayout(branchesLayout);
      miscControlsLayout->addLayout(saveLoadStateLayout);

      QWidget* miscControlsWidget = new QWidget();
      miscControlsWidget->setContentsMargins(0, 0, 0, 0);
      miscControlsWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
      miscControlsWidget->setLayout(miscControlsLayout);

      QVBoxLayout* leftLayout = new QVBoxLayout();
      leftLayout->addWidget(miscControlsWidget, 0, Qt::AlignLeft);
      leftLayout->addWidget(labelSkillpoints);
      leftLayout->addWidget(scrollAreaStats, 1);

      QWidget* leftWidget = new QWidget();
      leftWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
      leftWidget->setContentsMargins(0, 0, 0, 0);
      leftWidget->setObjectName("WidgetLeftPanel");
      leftWidget->setLayout(leftLayout);

      QHBoxLayout* centralLayout = new QHBoxLayout();
      centralLayout->addWidget(leftWidget, 1);
      centralLayout->addWidget(rightWidget);

      QHBoxLayout* consoleLayout = new QHBoxLayout();
      consoleLayout->setContentsMargins(0, 0, 0, 0);
      consoleLayout->addWidget(luaConsoleEdit);
      consoleLayout->addWidget(buttonStartStop);

      QVBoxLayout* centralVLayout = new QVBoxLayout();
      centralVLayout->addLayout(consoleLayout);
      centralVLayout->addLayout(centralLayout, 1);

      base->setLayout(centralVLayout);
    }
  };

  DebugWindow::DebugWindow(
    osgHelper::ioc::Injector& injector,
    QWidget* parent)
    : QDialog(parent)
    , m(new Impl(injector, this))
  {
    setWindowTitle("Debug Window");
    setGeometry(10, 10, 800, 800);

    m->setupUi();

    m->notifyDay = m->oDay->connect([this](int day)
    {
      m->updateCountryInfo();
    });

    m->notifySkillpoints = m->oNumSkillPoints->connectAndNotify([this](int skillPoints)
    {
      m->labelSkillpoints->setText(QString("Skill Points: %1").arg(skillPoints));
    });

    m->notifySelectedCountry = m->countryOverlay->getOSelectedCountryId()->connectAndNotify([this](int selected)
    {
      m->updateCountryInfo(selected);

      m->toggleCountryButton->setEnabled(selected > 0);
    });
  }

  DebugWindow::~DebugWindow()
  {
  }

  void DebugWindow::keyPressEvent(QKeyEvent* event)
  {
    if (event->key() != Qt::Key_Escape || !m->luaConsoleEdit->hasFocus())
      QDialog::keyPressEvent(event);
  }

  void DebugWindow::showEvent(QShowEvent* event)
  {
    m->updateCountryInfo();
  }

  void DebugWindow::closeEvent(QCloseEvent* event)
  {
    m->clearValueGraphs();
  }

  void DebugWindow::onCommandEntered(const QString& command)
  {
    std::string c = command.toStdString();
    m->simulation->getUpdateThread()->executeLockedTick([this, c]() { m->lua->executeCodeString(c); });
  }
}
