#include "SkillsWidget.h"

#include "core/Observables.h"
#include "simulation/ModelContainer.h"
#include "simulation/Simulation.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaSkillsTable.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QPointer>

namespace onep
{
  struct GridPosition
  {
    int column = 0;
    int row    = 0;
  };

  struct SkillToken
  {
    LuaSkill::Ptr         skill;
    QPointer<QPushButton> button;
  };

  struct SkillsWidget::Impl
  {
    Impl(osgGaming::Injector& injector, SkillsWidget* b)
      : base(b)
      , frameContent(nullptr)
      , modelContainer(injector.inject<ModelContainer>())
      , simulation(injector.inject<Simulation>())
    {
    }

    SkillsWidget*                     base;
    QFrame*                           frameContent;
    ModelContainer::Ptr               modelContainer;
    std::map<std::string, SkillToken> skillTokens;
    osgGaming::Observer<int>::Ptr     skillPointsObserver;
    Simulation::Ptr                   simulation;

    GridPosition insertSubBranch(QGridLayout* layout, const LuaSkill::Map& skills, GridPosition current,
                                 const QString& parent = "")
    {
      for (const auto& it : skills)
      {
        const auto& skill           = it.second;
        const auto& dependencies    = skill->getDependencies();
        const auto  hasDependencies = !dependencies.empty();
        const auto  cost            = skill->getCost();

        if ((!hasDependencies && parent.isEmpty()) || (hasDependencies && (QString::fromStdString(dependencies.front()) == parent)))
        {
          const auto name = QString::fromStdString(skill->getName());

          auto skillButton = new QPushButton(QString("%1 (%2)").arg(name).arg(cost));
          skillButton->setObjectName("ButtonSkill");
          skillButton->setCheckable(true);

          connect(skillButton, &QPushButton::toggled, [this, skillButton, skill, cost](bool checked)
          {
            if (!checked)
            {
              QSignalBlocker blocker(skillButton);
              skillButton->setChecked(true);
              return;
            }

            modelContainer->accessModel([this, skill, cost](const LuaModel::Ptr&)
            {
              skill->setIsActivated(true);
              simulation->paySkillPoints(cost);
            });
            
          });

		      skillTokens[it.first] = { it.second, skillButton };

          layout->addWidget(skillButton, current.row, current.column);

          current = insertSubBranch(layout, skills, { current.column, current.row + 1 }, name);
          current.column++;
        }
      }

      return { current.column, current.row - 1 };
    }

    QWidget* createBranchFrame(const LuaSkillBranch::Ptr& branch)
    {
      auto label = new QLabel(QString::fromStdString(branch->getName()));
      label->setStyleSheet("color: #fff;");

      auto treeLayout = new QGridLayout();

      const auto   skills = branch->getSkillsTable()->getSkills();
      GridPosition position;

      insertSubBranch(treeLayout, skills, position);

      auto treeFrame = new QFrame();
      treeFrame->setStyleSheet("border-color: 1px #008 solid;");
      treeFrame->setLayout(treeLayout);
      treeFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

      auto layout = new QVBoxLayout();
      layout->addWidget(label, 0, Qt::AlignLeft);
      layout->addWidget(treeFrame, 0, Qt::AlignLeft);
      layout->addStretch(1);

      auto frame = new QFrame();
      frame->setLayout(layout);

      return frame;
    }

    void setupUi()
    {
      auto layoutContent = new QHBoxLayout();

      auto branches = modelContainer->getModel()->getBranchesTable()->getBranches();
      for (const auto& branch : branches)
      {
         layoutContent->addWidget(createBranchFrame(branch.second));
      }

      frameContent = new QFrame();
      frameContent->setObjectName("FrameContent");
      frameContent->setLayout(layoutContent);
      
      auto mainLayout = new QGridLayout();
      mainLayout->addWidget(frameContent, 0, 0);

      base->setLayout(mainLayout);
    }
  };

  SkillsWidget::SkillsWidget(osgGaming::Injector& injector)
    : VirtualOverlay()
    , m(new Impl(injector, this))
  {
    m->setupUi();

    m->skillPointsObserver = injector.inject<ONumSkillPoints>()->connectAndNotify(osgGaming::Func<int>([this](int points)
    {
      for (const auto& token : m->skillTokens)
      {
        const auto& skill     = token.second.skill;
        const auto  activated = skill->getIsActivated();
        auto        enabled   = (activated || (skill->getCost() <= points));

        if (!activated)
		    {
          const auto& dependencies = skill->getDependencies();
          if (!dependencies.empty())
          {
            enabled = (enabled && m->skillTokens[dependencies.front()].skill->getIsActivated());
          }
		    }

        token.second.button->setEnabled(enabled);
      }
    }));
  }

  SkillsWidget::~SkillsWidget() = default;
}
