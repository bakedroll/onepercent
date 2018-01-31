#include "DebugWindow.h"

#include "core/QConnectFunctor.h"

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
    Impl(DebugWindow* b)
      : base(b)
      , toggleCountryButton(nullptr)
      , radioNoOverlay(nullptr)
      , labelStats(nullptr)
    {
      
    }

    DebugWindow* base;

    osg::ref_ptr<CountryOverlay> countryOverlay;
    osg::ref_ptr<BoundariesMesh> boundariesMesh;
    osg::ref_ptr<Simulation> simulation;

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
      printf("Took %d ms\n", d);
    }

    void updateCountryInfoText()
    {
      if (countryOverlay->getSelectedCountryId() == 0)
      {
        labelStats->setText("");
        return;
      }

      CountryData::Ptr country = countryOverlay->getSelectedCountryMesh()->getCountryData();

      std::string infoText = country->getCountryName() + "\n";
      country->getValues()->getContainer()->debugPrintToString(infoText);
      labelStats->setText(QString::fromStdString(infoText));
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
      for (int i = 0; i < NUM_SKILLBRANCHES; i++)
      {
        QCheckBox* checkBox = new QCheckBox(QString::fromStdString(branch_getStringFromType(i)));
        QRadioButton* radioButton = new QRadioButton(QObject::tr("Overlay"));
        radioGroup->addButton(radioButton);

        branchesLayout->addWidget(checkBox, 1 + i, 0);
        branchesLayout->addWidget(radioButton, 1 + i, 1);

        QConnectBoolFunctor::connect(checkBox, SIGNAL(clicked(bool)), [=](bool checked)
        {
          osg::ref_ptr<CountryData> selectedCountry = countryOverlay->getSelectedCountryMesh()->getCountryData();
          if (!selectedCountry.valid())
            return;

          selectedCountry->setSkillBranchActivated(i, checked);
        });

        QConnectBoolFunctor::connect(radioButton, SIGNAL(clicked(bool)), [=](bool checked)
        {
          countryOverlay->setHighlightedSkillBranch(BranchType(i));
        });

        selectedCountryIdObservers.push_back(countryOverlay->getSelectedCountryIdObservable()->connectAndNotify(osgGaming::Func<int>([=](int selected)
        {
          if (selected > 0)
          {
            checkBox->setChecked(countryOverlay->getCountryMesh(selected)->getCountryData()->getSkillBranchActivated(i));
          }
          else
          {
            checkBox->setChecked(false);
          }

          checkBox->setEnabled(selected > 0);
        })));

        CountryMesh::Map& countryMeshs = countryOverlay->getCountryMeshs();
        for (CountryMesh::Map::iterator it = countryMeshs.begin(); it != countryMeshs.end(); ++it)
        {
          skillBranchActivatedObservers.push_back(it->second->getCountryData()->getSkillBranchActivatedObservable(i)->connect(osgGaming::Func<bool>([=](bool activated)
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
      for (int i = 0; i < NUM_SKILLBRANCHES; i++)
      {
        // Skills
        int nskills = simulation->getSkillBranch(BranchType(i))->getNumSkills();

        QLabel* label = new QLabel(QString::fromStdString(branch_getStringFromType(i)));
        rightLayout->addWidget(label);

        for (int j = 0; j < nskills; j++)
        {
          Skill::Ptr skill = simulation->getSkillBranch(BranchType(i))->getSkill(j);

          QCheckBox* checkBox = new QCheckBox(QString::fromStdString(skill->getName()));
          rightLayout->addWidget(checkBox);

          QConnectBoolFunctor::connect(checkBox, SIGNAL(clicked(bool)), [=](bool checked)
          {
            simulation->getSkillBranch(BranchType(i))->getSkill(j)->setActivated(checked);
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
    osg::ref_ptr<CountryOverlay> countryOverlay,
    osg::ref_ptr<BoundariesMesh> boundariesMesh,
    osg::ref_ptr<Simulation> simulation,
    QWidget* parent)
    : QDialog(parent)
    , m(new Impl(this))
  {
    setGeometry(100, 100, 500, 800);

    m->countryOverlay = countryOverlay;
    m->boundariesMesh = boundariesMesh;
    m->simulation = simulation;

    m->setupUi();

    m->notifyDay = m->simulation->getDayObs()->connect(osgGaming::Func<int>([this](int day)
    {
      m->updateCountryInfoText();
    }));

    m->notifySkillpoints = m->simulation->getSkillPointsObs()->connectAndNotify(osgGaming::Func<int>([this](int skillPoints)
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