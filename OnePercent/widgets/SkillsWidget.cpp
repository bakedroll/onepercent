#include "SkillsWidget.h"

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

namespace onep
{
  struct SkillsWidget::Impl
  {
    Impl(osgGaming::Injector& injector, SkillsWidget* b)
      : base(b)
    {}

    SkillsWidget* base;
    QFrame* frameContent;

    void setupUi()
    {
      QLabel* label = new QLabel("test");
      QPushButton* test1 = new QPushButton("bla");
      QPushButton* test2 = new QPushButton("bla");
      QPushButton* test3 = new QPushButton("bla");

      QVBoxLayout* layoutContent = new QVBoxLayout();
      layoutContent->addWidget(label);
      layoutContent->addWidget(test1);
      layoutContent->addWidget(test2);
      layoutContent->addWidget(test3);

      frameContent = new QFrame();
      frameContent->setObjectName("FrameContent");
      frameContent->setLayout(layoutContent);
      
      QGridLayout* mainLayout = new QGridLayout();
      mainLayout->addWidget(frameContent, 0, 0);

      base->setLayout(mainLayout);
    }
  };

  SkillsWidget::SkillsWidget(osgGaming::Injector& injector)
    : VirtualOverlay()
    , m(new Impl(injector, this))
  {
    m->setupUi();
  }

  SkillsWidget::~SkillsWidget()
  {
  }
}