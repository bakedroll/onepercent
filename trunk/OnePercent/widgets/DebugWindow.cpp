#include "DebugWindow.h"

#include "core/QConnectFunctor.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>
#include <QLabel>

#include <chrono>

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

    QPushButton* toggleCountryButton;

    osg::ref_ptr<GlobeOverviewWorld> world;
    CountryMesh::Map selectedCountries;

    osgGaming::Observer<int>::Ptr notifySelectedCountry;
    osgGaming::Observer<int>::Ptr notifyDay;

    QRadioButton* radioNoOverlay;
    QLabel* labelStats;

    std::vector<osgGaming::Observer<int>::Ptr> selectedCountryIdObservers;
    std::vector<osgGaming::Observer<bool>::Ptr> skillBranchActivatedObservers;

    void toggleCountry()
    {
      auto start_time = std::chrono::high_resolution_clock::now();

      CountryMesh::Ptr mesh = world->getGlobeModel()->getCountryOverlay()->getSelectedCountryMesh();
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

      world->getGlobeModel()->getBoundariesMesh()->makeCountryBoundaries(meshlist, osg::Vec3f(1.0f, 0.5, 0.1f), 0.03f);

      auto duration = std::chrono::high_resolution_clock::now() - start_time;

      long d = std::chrono::duration_cast<std::chrono::milliseconds> (duration).count();
      printf("Took %d ms\n", d);
    }

    void updateCountryInfoText()
    {
      if (world->getGlobeModel()->getCountryOverlay()->getSelectedCountryId() == 0)
      {
        labelStats->setText("");
        return;
      }

      CountryData::Ptr country = world->getGlobeModel()->getCountryOverlay()->getSelectedCountryMesh()->getCountryData();

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
          world->getGlobeModel()->getCountryOverlay()->clearHighlightedCountries();
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
          osg::ref_ptr<CountryData> selectedCountry = world->getGlobeModel()->getCountryOverlay()->getSelectedCountryMesh()->getCountryData();
          if (!selectedCountry.valid())
            return;

          selectedCountry->setSkillBranchActivated(i, checked);
        });

        QConnectBoolFunctor::connect(radioButton, SIGNAL(clicked(bool)), [=](bool checked)
        {
          world->getGlobeModel()->getCountryOverlay()->setHighlightedSkillBranch(BranchType(i));
        });

        selectedCountryIdObservers.push_back(world->getGlobeModel()->getCountryOverlay()->getSelectedCountryIdObservable()->connectAndNotify(osgGaming::Func<int>([=](int selected)
        {
          if (selected > 0)
          {
            checkBox->setChecked(world->getGlobeModel()->getCountryOverlay()->getCountryMesh(selected)->getCountryData()->getSkillBranchActivated(i));
          }
          else
          {
            checkBox->setChecked(false);
          }

          checkBox->setEnabled(selected > 0);
        })));

        CountryMesh::Map& countryMeshs = world->getGlobeModel()->getCountryOverlay()->getCountryMeshs();
        for (CountryMesh::Map::iterator it = countryMeshs.begin(); it != countryMeshs.end(); ++it)
        {
          skillBranchActivatedObservers.push_back(it->second->getCountryData()->getSkillBranchActivatedObservable(i)->connect(osgGaming::Func<bool>([=](bool activated)
          {
            if (it->first == world->getGlobeModel()->getCountryOverlay()->getSelectedCountryId())
              checkBox->setChecked(activated);
          })));
        }
      }

      labelStats = new QLabel(QString());

      QVBoxLayout* rightLayout = new QVBoxLayout();

      // Skills
      for (int i = 0; i < NUM_SKILLBRANCHES; i++)
      {
        // Skills
        int nskills = world->getSimulation()->getSkillBranch(BranchType(i))->getNumSkills();

        QLabel* label = new QLabel(QString::fromStdString(branch_getStringFromType(i)));
        rightLayout->addWidget(label);

        for (int j = 0; j < nskills; j++)
        {
          Skill::Ptr skill = world->getSimulation()->getSkillBranch(BranchType(i))->getSkill(j);

          QCheckBox* checkBox = new QCheckBox(QString::fromStdString(skill->getName()));
          rightLayout->addWidget(checkBox);

          QConnectBoolFunctor::connect(checkBox, SIGNAL(clicked(bool)), [=](bool checked)
          {
            world->getSimulation()->getSkillBranch(BranchType(i))->getSkill(j)->setActivated(checked);
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

  DebugWindow::DebugWindow(osg::ref_ptr<GlobeOverviewWorld> world, QWidget* parent)
    : QDialog(parent)
    , m(new Impl(this))
  {
    setGeometry(100, 100, 500, 800);

    m->world = world;

    m->setupUi();

    m->notifyDay = m->world->getSimulation()->getDayObs()->connect(osgGaming::Func<int>([this](int day)
    {
      m->updateCountryInfoText();
    }));

    m->notifySelectedCountry = m->world->getGlobeModel()->getCountryOverlay()->getSelectedCountryIdObservable()->connectAndNotify(osgGaming::Func<int>([this](int selected)
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