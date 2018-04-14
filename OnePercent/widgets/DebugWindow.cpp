#include "DebugWindow.h"

#include "core/Macros.h"
#include "core/Observables.h"
#include "core/QConnectFunctor.h"
#include "simulation/Simulation.h"
#include "simulation/SimulationStateContainer.h"
#include "simulation/SimulationState.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>
#include <QLabel>

#include <chrono>
#include <nodes/CountryOverlay.h>
#include <nodes/BoundariesMesh.h>

namespace onep
{
  struct DebugWindow::Impl
  {
    Impl(osgGaming::Injector& injector, DebugWindow* b)
      : base(b)
      , countryOverlay(injector.inject<CountryOverlay>())
      , boundariesMesh(injector.inject<BoundariesMesh>())
      , simulation(injector.inject<Simulation>())
      , skillsContainer(injector.inject<SkillsContainer>())
      , stateContainer(injector.inject<SimulationStateContainer>())
      , oDay(injector.inject<ODay>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
      , toggleCountryButton(nullptr)
      , radioNoOverlay(nullptr)
      , labelStats(nullptr)
    {
    }

    DebugWindow* base;

    osg::ref_ptr<CountryOverlay> countryOverlay;
    osg::ref_ptr<BoundariesMesh> boundariesMesh;
    osg::ref_ptr<Simulation> simulation;
    osg::ref_ptr<SkillsContainer> skillsContainer;
    osg::ref_ptr<SimulationStateContainer> stateContainer;

    ODay::Ptr oDay;
    ONumSkillPoints::Ptr oNumSkillPoints;

    QPushButton* toggleCountryButton;
    CountryMesh::Map selectedCountries;

    osgGaming::Observer<int>::Ptr notifySelectedCountry;
    osgGaming::Observer<int>::Ptr notifyDay;
    osgGaming::Observer<int>::Ptr notifySkillpoints;

    QRadioButton* radioNoOverlay;
    QLabel* labelStats;
    QLabel* labelSkillpoints;

    std::vector<osgGaming::Observer<int>::Ptr> selectedCountryIdObservers;
    std::vector<osgGaming::Observer<bool>::Ptr> skillBranchActivatedObservers;

    void toggleCountry()
    {
      auto start_time = std::chrono::high_resolution_clock::now();

      CountryMesh::Ptr mesh = countryOverlay->getSelectedCountryMesh();
      if (!mesh.valid())
        return;

      CountryMesh::Map::iterator it = selectedCountries.find(mesh->getCountryData()->getId());
      if (it == selectedCountries.end())
        selectedCountries[mesh->getCountryData()->getId()] = mesh;
      else
        selectedCountries.erase(it);

      CountryMesh::List meshlist;
      for (CountryMesh::Map::iterator cit = selectedCountries.begin(); cit != selectedCountries.end(); ++cit)
        meshlist.push_back(cit->second);

      boundariesMesh->makeCountryBoundaries(meshlist, osg::Vec3f(1.0f, 0.5, 0.1f), 0.03f);

      auto duration = std::chrono::high_resolution_clock::now() - start_time;

      long d = std::chrono::duration_cast<std::chrono::milliseconds> (duration).count();

      OSGG_QLOG_DEBUG(QString("Took %1 ms").arg(d));
    }

    void updateCountryInfoText()
    {
      int selectedId = countryOverlay->getSelectedCountryId();

      if (selectedId == 0)
      {
        labelStats->setText("");
        return;
      }

      CountryState::Map& countryStates = stateContainer->getState()->getCountryStates();
      if (countryStates.count(selectedId) == 0)
        return;

      CountryData::Ptr country = countryOverlay->getSelectedCountryMesh()->getCountryData();
      CountryState::Ptr countryState = countryStates[selectedId];

      CountryState::ValuesMap& values = countryState->getValuesMap();
      CountryState::BranchValuesMap& branchValues = countryState->getBranchValuesMap();

      QString infoText = QString("%1 (%2)\n").arg(QString::fromLocal8Bit(country->getCountryName().c_str())).arg(country->getId());

      for (CountryState::ValuesMap::iterator it = values.begin(); it != values.end(); ++it)
        infoText += QString("%1: %2\n").arg(it->first.c_str()).arg(it->second);

      for (CountryState::BranchValuesMap::iterator it = branchValues.begin(); it != branchValues.end(); ++it)
        for (CountryState::ValuesMap::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
          infoText += QString("%1 %2: %3\n").arg(vit->first.c_str()).arg(it->first.c_str()).arg(vit->second);

      labelStats->setText(infoText);
    }

    void setupUi()
    {
      toggleCountryButton = new QPushButton("Toggle selected country");

      QConnectFunctor::connect(toggleCountryButton, SIGNAL(clicked()), [this]()
      {
        toggleCountry();
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
          osg::ref_ptr<CountryData> selectedCountry = countryOverlay->getSelectedCountryMesh()->getCountryData();
          if (!selectedCountry.valid())
            return;

          int cid = selectedCountry->getId();
          CountryState::Ptr cstate = stateContainer->getState()->getCountryStates()[cid];

          cstate->setBranchActivated(name.c_str(), checked);
        });

        QConnectBoolFunctor::connect(radioButton, SIGNAL(clicked(bool)), [=](bool checked)
        {
          countryOverlay->setHighlightedSkillBranch(i);
        });

        selectedCountryIdObservers.push_back(countryOverlay->getSelectedCountryIdObservable()->connectAndNotify(osgGaming::Func<int>([=](int selected)
        {
          if (selected > 0)
          {
            CountryState::Ptr cstate = stateContainer->getState()->getCountryStates()[selected];

            checkBox->setChecked(cstate->getBranchActivated(name.c_str()));
          }
          else
          {
            checkBox->setChecked(false);
          }

          checkBox->setEnabled(selected > 0);
        })));

        ONEP_FOREACH(CountryState::Map, it, stateContainer->getState()->getCountryStates())
        {
          skillBranchActivatedObservers.push_back(it->second->getOActivatedBranch(name.c_str())->connect(osgGaming::Func<bool>([=](bool activated)
          {
            if (it->first == countryOverlay->getSelectedCountryId())
              checkBox->setChecked(activated);
          })));
        }
      }

      labelStats = new QLabel(QString());
      labelSkillpoints = new QLabel(QString());
      labelSkillpoints->setObjectName("LabelSkillPoints");

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
          Skill::Ptr skill = skillBranch->getSkill(j);

          QCheckBox* checkBox = new QCheckBox(QString::fromStdString(skill->getName()));
          rightLayout->addWidget(checkBox);

          QConnectBoolFunctor::connect(checkBox, SIGNAL(clicked(bool)), [=](bool checked)
          {
            skillBranch->getSkill(j)->getObActivated()->set(checked);
          });
        }
      }

      rightLayout->addStretch(1);

      QWidget* rightWidget = new QWidget();
      rightWidget->setContentsMargins(0, 0, 0, 0);
      rightWidget->setObjectName("WidgetRightPanel");
      rightWidget->setLayout(rightLayout);

      QVBoxLayout* leftLayout = new QVBoxLayout();
      leftLayout->addWidget(toggleCountryButton);
      leftLayout->addLayout(branchesLayout);
      leftLayout->addWidget(labelSkillpoints);
      leftLayout->addWidget(labelStats);
      leftLayout->addStretch(1);

      QWidget* leftWidget = new QWidget();
      leftWidget->setContentsMargins(0, 0, 0, 0);
      leftWidget->setObjectName("WidgetLeftPanel");
      leftWidget->setLayout(leftLayout);

      QHBoxLayout* centralLayout = new QHBoxLayout();
      centralLayout->addWidget(leftWidget);
      centralLayout->addStretch(1);
      centralLayout->addWidget(rightWidget);

      base->setLayout(centralLayout);
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
      m->updateCountryInfoText();
    }));

    m->notifySkillpoints = m->oNumSkillPoints->connectAndNotify(osgGaming::Func<int>([this](int skillPoints)
    {
      m->labelSkillpoints->setText(QString("Skill Points: %1").arg(skillPoints));
    }));

    m->notifySelectedCountry = m->countryOverlay->getSelectedCountryIdObservable()->connectAndNotify(osgGaming::Func<int>([this](int selected)
    {
      m->updateCountryInfoText();

      m->toggleCountryButton->setEnabled(selected > 0);
      m->radioNoOverlay->setChecked(selected > 0);
    }));
  }

  DebugWindow::~DebugWindow()
  {
  }
}