#include "CountryMenuWidget.h"

#include "core/Observables.h"
#include "core/QConnectFunctor.h"

#include "simulation/Simulation.h"
#include "simulation/SimulationStateContainer.h"
#include "simulation/SimulationState.h"
#include "simulation/SkillBranch.h"
#include "simulation/UpdateThread.h"

#include <osgGaming/Helper.h>

#include <QPushButton>

namespace onep
{
	struct CountryMenuWidget::Impl
	{
    Impl(osgGaming::Injector& injector)
      : simulation(injector.inject<Simulation>())
      , skillsContainer(injector.inject<SkillsContainer>())
      , stateContainer(injector.inject<SimulationStateContainer>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
    {}

    Country::Ptr country;

    Simulation::Ptr simulation;
    SkillsContainer::Ptr skillsContainer;
    SimulationStateContainer::Ptr stateContainer;

    std::vector<QPushButton*> buttons;

    ONumSkillPoints::Ptr oNumSkillPoints;

    osgGaming::Observer<int>::Ptr notifySkillPoints;
    std::vector<osgGaming::Observer<bool>::Ptr> notifiesActivated;

    void updateUi()
    {
      notifiesActivated.clear();

      if (!country.valid())
        return;

      int cid = country->getId();

      stateContainer->accessState([=](SimulationState::Ptr state)
      {
        if (state->getCountryStates().count(cid) == 0)
          return;

        CountryState::Ptr cstate = state->getCountryStates()[cid];

        int n = skillsContainer->getNumBranches();
        for (int i = 0; i < n; i++)
        {
          SkillBranch::Ptr branch = skillsContainer->getBranchByIndex(i);
          std::string name = branch->getBranchName();

          notifiesActivated.push_back(cstate->getOActivatedBranch(name.c_str())->connectAndNotify(osgGaming::Func<bool>([=](bool activated)
          {
            if (activated)
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
          })));

        }
      });

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

      QConnectFunctor::connect(button, SIGNAL(clicked()), [=]()
      {
        if (!m->country.valid())
          return;

        int costs = branch->getCost();
        if (!m->simulation->paySkillPoints(costs))
          return;

        int cid = m->country->getId();

        // schedule task
        m->simulation->getUpdateThread()->executeLuaTask([=]()
        {
          m->stateContainer->accessState([=](SimulationState::Ptr state)
          {
            state->getCountryState(cid)->setBranchActivated(name.c_str(), true);
          });
        });

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

  void CountryMenuWidget::setCountry(Country::Ptr country)
  {
    m->country = country;
    m->updateUi();
  }
}