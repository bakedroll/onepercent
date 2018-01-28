#include "CountryMenuWidget.h"

#include "core/Globals.h"
#include "core/QConnectFunctor.h"

#include <osgGaming/Helper.h>

#include <QPushButton>

namespace onep
{
	struct CountryMenuWidget::Impl
	{
    Impl(Simulation::Ptr s)
      : simulation(s)
    {}

    CountryMesh::Ptr countryMesh;
    Simulation::Ptr simulation;
    std::vector<QPushButton*> buttons;

    osgGaming::Observer<int>::Ptr notifySkillPoints;

    int getBranchCosts(int branch)
    {
      switch (branch) {
      case BRANCH_CONTROL:    return 50;
      case BRANCH_BANKS:      return 120;
      case BRANCH_CONCERNS:   return 120;
      case BRANCH_MEDIA:      return 50;
      case BRANCH_POLITICS:   return 50;
      case BRANCH_UNDEFINED:
      default:
        assert(false);
      }

      return 0;
    }

    void updateUi()
    {
      if (!countryMesh.valid())
        return;

      for (int i = 0; i < NUM_SKILLBRANCHES; i++)
      {
        if (countryMesh->getCountryData()->getSkillBranchActivated(i))
        {
          buttons[i]->setText(QString("%1\n%2").arg(QString::fromStdString(branch_getStringFromType(i))).arg(tr("(Unlocked)")));
          buttons[i]->setEnabled(false);
        }
        else
        {
          int costs = getBranchCosts(i);

          buttons[i]->setText(QString("%1\n(%2 SP to unlock)").arg(QString::fromStdString(branch_getStringFromType(i))).arg(costs));
          buttons[i]->setEnabled(simulation->getSkillPointsObs()->get() >= costs);
        }
      }
    }
	};

  CountryMenuWidget::CountryMenuWidget(Simulation::Ptr simulation)
		: VirtualOverlay()
		, m(new Impl(simulation))
	{
    QSize size(400, 400);
    QSize buttonSize(100, 100);

    setFixedSize(size);

    for (int i = 0; i<NUM_SKILLBRANCHES; i++)
    {
      QPushButton* button = new QPushButton(QString::fromStdString(branch_getStringFromType(i)));
      button->setParent(this);
      button->setFixedSize(buttonSize);
      button->setGeometry(
        size.width() / 2 + sin(float(i) * C_2PI / float(NUM_SKILLBRANCHES)) * 110 - buttonSize.width() / 2,
        size.height() / 2 + cos(float(i) * C_2PI / float(NUM_SKILLBRANCHES)) * 110 - buttonSize.height() / 2,
        buttonSize.width(),
        buttonSize.height());

      m->buttons.push_back(button);

      QConnectFunctor::connect(button, SIGNAL(clicked()), [this, i]()
      {
        if (!m->countryMesh.valid())
          return;

        int costs = m->getBranchCosts(i);
        if (!m->simulation->paySkillPoints(costs))
          return;

        m->countryMesh->getCountryData()->setSkillBranchActivated(i, true);

        if (!m->simulation->running())
          m->simulation->start();
      });
    }

    m->notifySkillPoints = m->simulation->getSkillPointsObs()->connect(osgGaming::Func<int>([this](int skillPoints)
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