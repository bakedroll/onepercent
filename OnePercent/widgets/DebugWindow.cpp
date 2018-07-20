#include "DebugWindow.h"

#include "core/Macros.h"
#include "core/Multithreading.h"
#include "core/Observables.h"
#include "core/QConnectFunctor.h"
#include "simulation/CountriesContainer.h"
#include "simulation/Country.h"
#include "simulation/Simulation.h"
#include "simulation/SimulationStateContainer.h"
#include "simulation/SimulationState.h"
#include "simulation/UpdateThread.h"

#include <osgGaming/Macros.h>

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

    QConnectFunctor::connect(this, SIGNAL(returnPressed()), [&]()
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
    Impl(osgGaming::Injector& injector, DebugWindow* b)
      : base(b)
      , lua(injector.inject<LuaStateManager>())
      , countryOverlay(injector.inject<CountryOverlay>())
      , boundariesMesh(injector.inject<BoundariesMesh>())
      , simulation(injector.inject<Simulation>())
      , skillsContainer(injector.inject<SkillsContainer>())
      , countriesContainer(injector.inject<CountriesContainer>())
      , stateContainer(injector.inject<SimulationStateContainer>())
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

    osg::ref_ptr<LuaStateManager> lua;
    osg::ref_ptr<CountryOverlay> countryOverlay;
    osg::ref_ptr<BoundariesMesh> boundariesMesh;
    osg::ref_ptr<Simulation> simulation;
    osg::ref_ptr<SkillsContainer> skillsContainer;
    osg::ref_ptr<CountriesContainer> countriesContainer;
    osg::ref_ptr<SimulationStateContainer> stateContainer;

    ODay::Ptr oDay;
    ONumSkillPoints::Ptr oNumSkillPoints;

    QPushButton* toggleCountryButton;
    CountryMesh::Map selectedCountries;

    osgGaming::Observer<int>::Ptr notifySelectedCountry;
    osgGaming::Observer<int>::Ptr notifyDay;
    osgGaming::Observer<int>::Ptr notifySkillpoints;
    osgGaming::Observer<bool>::Ptr notifyRunningValues;
    osgGaming::Observer<bool>::Ptr notifyRunningButton;

    QScrollArea* scrollAreaStats;
    QWidget* widgetStats;
    QVBoxLayout* layoutStats;
    QRadioButton* radioNoOverlay;
    QLabel* labelSkillpoints;
    QLabel* labelCountry;
    ConsoleEdit* luaConsoleEdit;
    QPushButton* buttonStartStop;
    QCheckBox* checkBoxEnableGraph;

    std::vector<osgGaming::Observer<int>::Ptr> selectedCountryIdObservers;
    std::vector<osgGaming::Observer<bool>::Ptr> skillBranchActivatedObservers;
    std::vector<osgGaming::Observer<bool>::Ptr> skillActivatedObservers;

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
    typedef std::map<std::string, ValueWidgets> BranchValueWidgets;

    ValueWidgets valueWidgets;
    BranchValueWidgets branchValueWidgets;

    bool bWireframe;
    float borderThickness;

    void updateBoundaries()
    {
      auto start_time = std::chrono::high_resolution_clock::now();
      boundariesMesh->makeCountryBoundaries(selectedCountries, osg::Vec3f(1.0f, 0.5, 0.1f), borderThickness, bWireframe);
      auto duration = std::chrono::high_resolution_clock::now() - start_time;
      long d = std::chrono::duration_cast<std::chrono::milliseconds> (duration).count();

      OSGG_QLOG_DEBUG(QString("Took %1 ms").arg(d));
    }

    void toggleCountry()
    {
      int id = countryOverlay->getSelectedCountryId();

      if (id == 0)
        return;

      CountryMesh::Ptr mesh = countryOverlay->getSelectedCountryMesh();
      assert_return(mesh.valid());

      CountryMesh::Map::iterator it = selectedCountries.find(id);
      if (it == selectedCountries.end())
        selectedCountries[id] = mesh;
      else
        selectedCountries.erase(it);

      updateBoundaries();
    }

    ValueWidget createValueWidgets(const QString& labelText, std::function<void(float, CountryState::Ptr)> setFunc)
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
      QConnectFunctor::connect(widget.edit, SIGNAL(textEdited(QString)), [button]()
      {
        button->setEnabled(true);
      });

      QLineEdit* edit = widget.edit;
      QConnectFunctor::connect(widget.buttonSet, SIGNAL(clicked()), [=]()
      {
        int id = countryOverlay->getSelectedCountryId();
        if (id == 0) return;

        stateContainer->accessState([=](SimulationState::Ptr state)
        {
          CountryState::Ptr cstate = state->getCountryState(id);

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

      for (BranchValueWidgets::iterator it = branchValueWidgets.begin(); it != branchValueWidgets.end(); ++it)
      {
        for (ValueWidgets::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
        {
          vit->second.latestValues.clear();
          vit->second.pixGraph.fill(QColor(255, 255, 255));
          vit->second.labelGraph->setPixmap(vit->second.pixGraph);
        }
      }
    }

    void updateValueGraph(ValueWidget& widget, float value)
    {
      int w = widget.pixGraph.width();
      int h = widget.pixGraph.height();

      widget.latestValues.push_front(osg::clampBetween<float>(value, 0.0f, 1.0f));
      if (widget.latestValues.size() > w)
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
      // cancel update if DebugWindow is not shown
      if (!base->isVisible())
        return;

      int selectedId = countryOverlay->getSelectedCountryId();

      widgetStats->setVisible(selectedId > 0);
      if (selectedId == 0)
        return;

      stateContainer->accessState([=](SimulationState::Ptr state)
      {
        CountryState::Map& countryStates = state->getCountryStates();
        if (countryStates.count(selectedId) == 0)
          return;

        Country::Ptr country = countriesContainer->getCountry(selectedId);
        CountryState::Ptr countryState = countryStates[selectedId];

        CountryState::ValuesMap& values = countryState->getValuesMap();
        CountryState::BranchValuesMap& branchValues = countryState->getBranchValuesMap();

        if (layoutStats == nullptr)
        {
          layoutStats = new QVBoxLayout();
          layoutStats->setContentsMargins(10, 10, 10, 10);
          layoutStats->setSpacing(0);
          widgetStats->setLayout(layoutStats);

          scrollAreaStats->setWidget(widgetStats);

          labelCountry = new QLabel();

          layoutStats->addWidget(labelCountry);

          for (CountryState::ValuesMap::iterator it = values.begin(); it != values.end(); ++it)
          {
            std::string name = it->first;
            ValueWidget widget = createValueWidgets(QString("%1").arg(it->first.c_str()), [=](float value, CountryState::Ptr cstate)
            {
              simulation->getUpdateThread()->executeLockedLuaState([cstate, name, value]()
              {
                cstate->getValuesMap()[name] = value;
                cstate->writeValues();
              });
            });
            valueWidgets[it->first] = widget;
          }

          for (CountryState::BranchValuesMap::iterator it = branchValues.begin(); it != branchValues.end(); ++it)
          {
            for (CountryState::ValuesMap::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
            {
              std::string branchName = it->first;
              std::string name = vit->first;
              ValueWidget widget = createValueWidgets(QString("%1 %2\n").arg(vit->first.c_str()).arg(it->first.c_str()), [=](float value, CountryState::Ptr cstate)
              {
                simulation->getUpdateThread()->executeLockedLuaState([cstate, branchName, name, value]()
                {
                  cstate->getBranchValuesMap()[branchName][name] = value;
                  cstate->writeBranchValues();
                });
              });
              branchValueWidgets[it->first][vit->first] = widget;
            }
          }

          notifyRunningValues = simulation->getORunning()->connectAndNotify(osgGaming::Func<bool>([this](bool running)
          {
            for (ValueWidgets::iterator it = valueWidgets.begin(); it != valueWidgets.end(); ++it)
              it->second.edit->setEnabled(!running);

            for (BranchValueWidgets::iterator it = branchValueWidgets.begin(); it != branchValueWidgets.end(); ++it)
              for (ValueWidgets::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
                vit->second.edit->setEnabled(!running);
          }));
        }

        labelCountry->setText(QString("%1 (%2)").arg(QString::fromLocal8Bit(country->getName().c_str())).arg(country->getId()));

        for (CountryState::ValuesMap::iterator it = values.begin(); it != values.end(); ++it)
        {
          valueWidgets[it->first].edit->setText(QString::number(it->second));
          valueWidgets[it->first].progressBar->setValue(osg::clampBetween<int>(int(it->second * 100.0f), 0, 100));

          if (checkBoxEnableGraph->isChecked())
            updateValueGraph(valueWidgets[it->first], it->second);
        }

        for (CountryState::BranchValuesMap::iterator it = branchValues.begin(); it != branchValues.end(); ++it)
        {
          for (CountryState::ValuesMap::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
          {
            branchValueWidgets[it->first][vit->first].edit->setText(QString::number(vit->second));
            branchValueWidgets[it->first][vit->first].progressBar->setValue(osg::clampBetween<int>(int(vit->second * 100.0f), 0, 100));

            if (checkBoxEnableGraph->isChecked())
              updateValueGraph(branchValueWidgets[it->first][vit->first], vit->second);
          }
        }

      });
    }

    void setupUi()
    {
      toggleCountryButton = new QPushButton("Toggle selected country");
      toggleCountryButton->setFocusPolicy(Qt::NoFocus);

      QCheckBox* checkBoxWireframe = new QCheckBox("Wireframe Borders");
      checkBoxWireframe->setChecked(bWireframe);
      checkBoxWireframe->setFocusPolicy(Qt::NoFocus);

      QDoubleSpinBox* spinboxThickness = new QDoubleSpinBox();
      spinboxThickness->setValue(borderThickness);
      spinboxThickness->setRange(0.01, 999.0);
      spinboxThickness->setSingleStep(0.01);

      QConnectFunctor::connect(toggleCountryButton, SIGNAL(clicked()), [this]()
      {
        toggleCountry();
      });

      QConnectBoolFunctor::connect(checkBoxWireframe, SIGNAL(toggled(bool)), [this](bool checked)
      {
        bWireframe = checked;
        updateBoundaries();
      });

      QConnectDoubleFunctor::connect(spinboxThickness, SIGNAL(valueChanged(double)), [this](double value)
      {
        borderThickness = float(value);
        updateBoundaries();
      });

      QGridLayout* branchesLayout = new QGridLayout();

      QButtonGroup* radioGroup = new QButtonGroup(base);

      radioNoOverlay = new QRadioButton(QObject::tr("No Overlay"));
      radioNoOverlay->setChecked(true);
      radioGroup->addButton(radioNoOverlay);

      QConnectBoolFunctor::connect(radioNoOverlay, SIGNAL(clicked(bool)), [=](bool checked)
      {
        if (checked)
          countryOverlay->clearHighlightedCountries();
      });

      branchesLayout->addWidget(radioNoOverlay, 0, 1);

      int n = skillsContainer->getNumBranches();
      for (int i = 0; i < n; i++)
      {
        std::string name = skillsContainer->getBranchByIndex(i)->getBranchName();

        QCheckBox* checkBox = new QCheckBox(QString::fromStdString(name));
        QRadioButton* radioButton = new QRadioButton(QObject::tr("Overlay"));
        radioGroup->addButton(radioButton);

        branchesLayout->addWidget(checkBox, 1 + i, 0);
        branchesLayout->addWidget(radioButton, 1 + i, 1);

        QConnectBoolFunctor::connect(checkBox, SIGNAL(clicked(bool)), [=](bool checked)
        {
          int cid = countryOverlay->getSelectedCountryId();
          if (cid == 0)
            return;

          // schedule task
          simulation->getUpdateThread()->executeLockedTick([=]()
          {
            stateContainer->accessState([=](SimulationState::Ptr state)
            {
              state->getCountryState(cid)->setBranchActivated(name.c_str(), checked);
            });
          });
        });

        QConnectBoolFunctor::connect(radioButton, SIGNAL(clicked(bool)), [=](bool checked)
        {
          countryOverlay->setHighlightedSkillBranch(i);
        });

        selectedCountryIdObservers.push_back(countryOverlay->getSelectedCountryIdObservable()->connectAndNotify(osgGaming::Func<int>([=](int selected)
        {
          if (selected > 0)
          {
            stateContainer->accessState([=](SimulationState::Ptr state)
            {
              checkBox->setChecked(state->getCountryState(selected)->getBranchActivated(name.c_str()));
            });
          }
          else
          {
            checkBox->setChecked(false);
          }

          checkBox->setEnabled(selected > 0);
        })));

        stateContainer->accessState([this, name, checkBox](SimulationState::Ptr state)
        {
          for(auto& it : state->getCountryStates())
          {
            int cid = it.first;

            skillBranchActivatedObservers.push_back(it.second->getOActivatedBranch(name.c_str())->connect(osgGaming::Func<bool>([=](bool activated)
            {
              Multithreading::uiExecuteOrAsync([=]()
              {
                if (cid == countryOverlay->getSelectedCountryId())
                  checkBox->setChecked(activated);
              });
            })));
          }
        });

      }

      luaConsoleEdit = new ConsoleEdit();
      buttonStartStop = new QPushButton();
      buttonStartStop->setFixedWidth(100);
      buttonStartStop->setFocusPolicy(Qt::NoFocus);

      connect(luaConsoleEdit, SIGNAL(commandEntered(QString)), base, SLOT(onCommandEntered(QString)));

      QConnectFunctor::connect(buttonStartStop, SIGNAL(clicked()), [&]()
      {
        if (simulation->running())
          simulation->stop();
        else
          simulation->start();
      });

      notifyRunningButton = simulation->getORunning()->connectAndNotify(osgGaming::Func<bool>([&](bool running)
      {
        if (running)
          buttonStartStop->setText("Stop simulation");
        else
          buttonStartStop->setText("Start simulation");
      }));

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
      for (int i = 0; i < n; i++)
      {
        SkillBranch::Ptr skillBranch = skillsContainer->getBranchByIndex(i);
        std::string name = skillBranch->getBranchName();

        // Skills
        int nskills = skillBranch->getNumSkills();

        QLabel* label = new QLabel(QString::fromStdString(name));
        rightLayout->addWidget(label);

        for (int j = 0; j < nskills; j++)
        {
          Skill::Ptr skill = skillBranch->getSkillByIndex(j);

          QCheckBox* checkBox = new QCheckBox(QString::fromStdString(skill->getDisplayName()));
          rightLayout->addWidget(checkBox);

          QConnectBoolFunctor::connect(checkBox, SIGNAL(clicked(bool)), [=](bool checked)
          {
            // No need to schedule thread task here. Skill branch will never be modified from Lua code.
            skillBranch->getSkillByIndex(j)->getObActivated()->set(checked);
          });

          skillActivatedObservers.push_back(skill->getObActivated()->connect(osgGaming::Func<bool>([=](bool activated)
          {
            checkBox->setChecked(activated);
          })));
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

      QConnectBoolFunctor::connect(checkBoxEnableGraph, SIGNAL(toggled(bool)), [this](bool checked)
      {
        if (!checked)
          clearValueGraphs();
      });

      QConnectFunctor::connect(loadStateButton, SIGNAL(clicked()), [this]()
      {
        QString filename = QFileDialog::getOpenFileName(base, "Load state", QDir::currentPath(), "State files (*.ste)");
        if (!filename.isEmpty())
        {
          simulation->loadState(filename.toStdString());
          updateCountryInfo();
        }
      });

      QConnectFunctor::connect(saveStateButton, SIGNAL(clicked()), [this]()
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
    osgGaming::Injector& injector,
    QWidget* parent)
    : QDialog(parent)
    , m(new Impl(injector, this))
  {
    setGeometry(100, 100, 500, 800);

    m->setupUi();

    m->notifyDay = m->oDay->connect(osgGaming::Func<int>([this](int day)
    {
      m->updateCountryInfo();
    }));

    m->notifySkillpoints = m->oNumSkillPoints->connectAndNotify(osgGaming::Func<int>([this](int skillPoints)
    {
      m->labelSkillpoints->setText(QString("Skill Points: %1").arg(skillPoints));
    }));

    m->notifySelectedCountry = m->countryOverlay->getSelectedCountryIdObservable()->connectAndNotify(osgGaming::Func<int>([this](int selected)
    {
      m->updateCountryInfo();

      m->toggleCountryButton->setEnabled(selected > 0);
      m->radioNoOverlay->setChecked(selected > 0);
    }));
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
    m->simulation->getUpdateThread()->executeLockedLuaState([this, c]() { m->lua->executeCode(c); });
  }
}