#include "CountryMenuWidget.h"

#include "core/Observables.h"
#include "core/QConnectFunctor.h"

#include "simulation/Simulation.h"
#include "simulation/SimulatedValuesContainer.h"
#include "simulation/SkillBranch.h"

#include <osgGaming/Helper.h>

#include <QPushButton>

namespace onep
{
	struct CountryMenuWidget::Impl
	{
    Impl(osgGaming::Injector& injector)
      : simulation(injector.inject<Simulation>())
      , skillsContainer(injector.inject<SkillsContainer>())
      , valuesContainer(injector.inject<SimulatedValuesContainer>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
    {}

    CountryMesh::Ptr countryMesh;

    Simulation::Ptr simulation;
    SkillsContainer::Ptr skillsContainer;
    SimulatedValuesContainer::Ptr valuesContainer;

    std::vector<QPushButton*> buttons;

    ONumSkillPoints::Ptr oNumSkillPoints;

    osgGaming::Observer<int>::Ptr notifySkillPoints;

    void updateUi()
    {
      if (!countryMesh.valid())
        return;

      int cid = countryMesh->getCountryData()->getId();
      if (valuesContainer->getState()->getCountryStates().count(cid) == 0)
        return;

      CountryState::Ptr cstate = valuesContainer->getState()->getCountryStates()[cid];

      int n = skillsContainer->getNumBranches();
      for (int i = 0; i < n; i++)
      {
        SkillBranch::Ptr branch = skillsContainer->getBranchByIndex(i);
        std::string name = branch->getBranchName();

        if (cstate->getBranchActivated(name.c_str()))
        {
          buttons[i]->setText(QString("%1\n%2").arg(QString::fromStdString(name)).arg(tr("(Unlocked)")));
          buttons[i]->setEnabled(false);
        }
        else
        {
          int costs = branch->getCost();

          buttons[i]->setText(QString("%1\n(%2 SP to unlock)").arg(QString::fromStdString(name)).arg(costs));
          buttons[i]->setEnabled(oNumSkillPoints->get() >= costs);
        }
      }
    }
	};

  CountryMenuWidget::CountryMenuWidget(osgGaming::Injector& injector)
		: VirtualOverlay()
		, m(new Impl(injector))
	{
    QSize size(400, 400);
    QSize buttonSize(100, 100);

    setFixedSize(size);

    int n = m->skillsContainer->getNumBranches();
    for (int i = 0; i<n; i++)
    {
      SkillBranch::Ptr branch = m->skillsContainer->getBranchByIndex(i);
      std::string name = branch->getBranchName();

      QPushButton* button = new QPushButton(QString::fromStdString(name));
      button->setParent(this);
      button->setFixedSize(buttonSize);
      button->setGeometry(
        size.width() / 2 + sin(float(i) * C_2PI / float(n)) * 110 - buttonSize.width() / 2,
        size.height() / 2 + cos(float(i) * C_2PI / float(n)) * 110 - buttonSize.height() / 2,
        buttonSize.width(),
        buttonSize.height());

      m->buttons.push_back(button);

      QConnectFunctor::connect(button, SIGNAL(clicked()), [this, i, branch, name]()
      {
        if (!m->countryMesh.valid())
          return;

        int costs = branch->getCost();
        if (!m->simulation->paySkillPoints(costs))
          return;

        int cid = m->countryMesh->getCountryData()->getId();
        CountryState::Ptr cstate = m->valuesContainer->getState()->getCountryStates()[cid];

        cstate->setBranchActivated(name.c_str(), true);

        if (!m->simulation->running())
          m->simulation->start();
      });
    }

    m->notifySkillPoints = m->oNumSkillPoints->connect(osgGaming::Func<int>([this](int skillPoints)
    {
      m->updateUi();
    }));
	}

	CountryMenuWidget::~CountryMenuWidget()
	{
	}

  void CountryMenuWidget::setCenterPosition(int x, int y)
	{
    QRect geo = geometry();
    setGeometry(x - geo.width() / 2, y - geo.height() / 2, geo.width(), geo.height());
	}

  void CountryMenuWidget::setCountryMesh(CountryMesh::Ptr countryMesh)
  {
    m->countryMesh = countryMesh;
    m->updateUi();
  }
}