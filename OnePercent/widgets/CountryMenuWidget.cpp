#include "CountryMenuWidget.h"

#include "core/Observables.h"
#include "core/QConnectFunctor.h"

#include "simulation/Simulation.h"

#include <osgGaming/Helper.h>

#include <QPushButton>

namespace onep
{
	struct CountryMenuWidget::Impl
	{
    Impl(osgGaming::Injector& injector)
      : simulation(injector.inject<Simulation>())
      , skillBranchContainer(injector.inject<SkillBranchContainer>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
    {}

    CountryMesh::Ptr countryMesh;

    Simulation::Ptr simulation;
    SkillBranchContainer::Ptr skillBranchContainer;

    std::vector<QPushButton*> buttons;

    ONumSkillPoints::Ptr oNumSkillPoints;

    osgGaming::Observer<int>::Ptr notifySkillPoints;

    void updateUi()
    {
      if (!countryMesh.valid())
        return;

      int n = skillBranchContainer->getNumBranches();
      for (int i = 0; i < n; i++)
      {
        SkillBranch::Ptr branch = skillBranchContainer->getBranchByIndex(i);
        std::string name = branch->getBranchName();

        if (countryMesh->getCountryData()->getSkillBranchActivated(i))
        {
          buttons[i]->setText(QString("%1\n%2").arg(QString::fromStdString(name)).arg(tr("(Unlocked)")));
          buttons[i]->setEnabled(false);
        }
        else
        {
          int costs = branch->getCosts();

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

    int n = m->skillBranchContainer->getNumBranches();
    for (int i = 0; i<n; i++)
    {
      SkillBranch::Ptr branch = m->skillBranchContainer->getBranchByIndex(i);
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

      QConnectFunctor::connect(button, SIGNAL(clicked()), [this, i, branch]()
      {
        if (!m->countryMesh.valid())
          return;

        int costs = branch->getCosts();
        if (!m->simulation->paySkillPoints(costs))
          return;

        m->countryMesh->getCountryData()->setSkillBranchActivated(i, true);

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