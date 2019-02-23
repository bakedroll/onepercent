#include "CountryMenuWidget.h"

#include "core/Multithreading.h"
#include "core/Observables.h"
#include "core/QConnectFunctor.h"
#include "nodes/CountryOverlay.h"
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
      : lua(injector.inject<LuaStateManager>())
      , simulation(injector.inject<Simulation>())
      , modelContainer(injector.inject<ModelContainer>())
      , countryOverlay(injector.inject<CountryOverlay>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
    {}

    LuaCountry::Ptr country;

    LuaStateManager::Ptr lua;
    Simulation::Ptr      simulation;
    ModelContainer::Ptr  modelContainer;
    CountryOverlay::Ptr  countryOverlay;

    std::vector<QPushButton*> buttons;

    ONumSkillPoints::Ptr oNumSkillPoints;

    osgGaming::Observer<int>::Ptr               notifySkillPoints;
    std::vector<osgGaming::Observer<bool>::Ptr> notifiesActivated;

    void updateUi()
    {
      notifiesActivated.clear();

      if (!country)
      {
        return;
      }

      auto cid = country->getId();

      modelContainer->accessModel([=](const LuaModel::Ptr& model)
      {
        if (model->getSimulationStateTable()->getCountryStates().count(cid) == 0)
        {
          return;
        }

        auto&       cstate   = model->getSimulationStateTable()->getCountryStates()[cid];
        const auto& branches = model->getBranchesTable()->getBranches();
        auto        index    = 0;

        for (const auto& branch : branches)
        {
          const auto& name = branch.second->getName();

          notifiesActivated.push_back(cstate->getBranchesActivatedTable()->getOBranchActivated(name)->connectAndNotify(
            osgGaming::Func<bool>([this, index, branch, name](bool activated)
          {
            Multithreading::uiExecuteOrAsync([this, index, branch, name, activated]()
            {
              if (activated)
              {
                buttons[index]->setText(QString("%1\n%2").arg(QString::fromStdString(name)).arg(tr("(Unlocked)")));
                buttons[index]->setEnabled(false);
              }
              else
              {
                auto costs = branch.second->getCost();

                buttons[index]->setText(QString("%1\n(%2 SP to unlock)").arg(QString::fromStdString(name)).arg(costs));
                buttons[index]->setEnabled(oNumSkillPoints->get() >= costs);
              }
            });
          })));

          index++;
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

    const auto& branches    = m->modelContainer->getModel()->getBranchesTable()->getBranches();
    auto        numBranches = static_cast<int>(branches.size());
    auto        index       = 0;

    for (const auto& branch : branches)
    {
      const auto& name = branch.second->getName();

      auto posX = size.width()  / 2 + static_cast<int>(sin(index * C_2PI / numBranches) * 110.0f) - buttonSize.width()  / 2;
      auto posY = size.height() / 2 + static_cast<int>(cos(index * C_2PI / numBranches) * 110.0f) - buttonSize.height() / 2;

      auto button = new QPushButton(QString::fromStdString(name));
      button->setParent(this);
      button->setFixedSize(buttonSize);
      button->setGeometry(posX, posY, buttonSize.width(), buttonSize.height());

      m->buttons.push_back(button);

      QConnectFunctor::connect(button, SIGNAL(clicked()), [=]()
      {
        if (!m->country)
        {
          return;
        }

        auto costs = branch.second->getCost();
        if (!m->simulation->paySkillPoints(costs))
        {
          return;
        }

        auto cid = m->country->getId();

        m->lua->safeExecute([this, &cid, &name]()
        {
          m->simulation->getUpdateThread()->executeLockedTick([this, &cid, &name]()
          {
            m->modelContainer->accessModel([&cid, &name](const LuaModel::Ptr& model)
            {
              model->getSimulationStateTable()->getCountryState(cid)->getBranchesActivatedTable()->setBranchActivated(name, true);
            });
          });
        });

        // todo: move to lua event
        m->countryOverlay->setCurrentOverlayBranchName(name);

        if (!m->simulation->running())
        {
          m->simulation->start();
        }
      });

      index++;
    }

    m->notifySkillPoints = m->oNumSkillPoints->connect(osgGaming::Func<int>([this](int skillPoints)
    {
      m->updateUi();
    }));
	}

	CountryMenuWidget::~CountryMenuWidget() = default;

  void CountryMenuWidget::setCenterPosition(int x, int y)
	{
    const auto& geo = geometry();
    setGeometry(x - geo.width() / 2, y - geo.height() / 2, geo.width(), geo.height());
	}

  void CountryMenuWidget::setCountry(const LuaCountry::Ptr& country)
  {
    m->country = country;
    m->updateUi();
  }
}
