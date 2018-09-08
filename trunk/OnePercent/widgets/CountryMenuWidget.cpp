#include "CountryMenuWidget.h"

#include "core/Multithreading.h"
#include "core/Observables.h"
#include "core/QConnectFunctor.h"

#include "simulation/Simulation.h"
#include "simulation/ModelContainer.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaSkillBranch.h"
#include "simulation/UpdateThread.h"

#include <osgGaming/Helper.h>

#include <QPushButton>

namespace onep
{
	struct CountryMenuWidget::Impl
	{
    Impl(osgGaming::Injector& injector)
      : simulation(injector.inject<Simulation>())
      , modelContainer(injector.inject<ModelContainer>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
    {}

    LuaCountry::Ptr country;

    Simulation::Ptr simulation;
    ModelContainer::Ptr modelContainer;

    std::vector<QPushButton*> buttons;

    ONumSkillPoints::Ptr oNumSkillPoints;

    osgGaming::Observer<int>::Ptr notifySkillPoints;
    std::vector<osgGaming::Observer<bool>::Ptr> notifiesActivated;

    void updateUi()
    {
      notifiesActivated.clear();

      if (!country)
        return;

      int cid = country->getId();

      modelContainer->accessModel([=](LuaModel::Ptr model)
      {
        if (model->getSimulationStateTable()->getCountryStates().count(cid) == 0)
          return;

        LuaCountryState::Ptr cstate = model->getSimulationStateTable()->getCountryStates()[cid];
        auto branchesTable = model->getBranchesTable();

        int n = branchesTable->getNumBranches();
        for (int i = 0; i < n; i++)
        {
          LuaSkillBranch::Ptr branch = branchesTable->getBranchByIndex(i);
          std::string name = branch->getBranchName();

          notifiesActivated.push_back(cstate->getBranchesActivatedTable()->getOBranchActivated(name.c_str())->connectAndNotify(osgGaming::Func<bool>([=](bool activated)
          {
            Multithreading::uiExecuteOrAsync([=]()
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
            });
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

    auto branchesTable = m->modelContainer->getModel()->getBranchesTable();

    int n = branchesTable->getNumBranches();
    for (int i = 0; i<n; i++)
    {
      LuaSkillBranch::Ptr branch = branchesTable->getBranchByIndex(i);
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
        if (!m->country)
          return;

        int costs = branch->getCost();
        if (!m->simulation->paySkillPoints(costs))
          return;

        int cid = m->country->getId();

        // schedule task
        m->simulation->getUpdateThread()->executeLockedTick([=]()
        {
          m->modelContainer->accessModel([=](LuaModel::Ptr model)
          {
            model->getSimulationStateTable()->getCountryState(cid)->getBranchesActivatedTable()->setBranchActivated(name.c_str(), true);
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

  void CountryMenuWidget::setCountry(LuaCountry::Ptr country)
  {
    m->country = country;
    m->updateUi();
  }
}