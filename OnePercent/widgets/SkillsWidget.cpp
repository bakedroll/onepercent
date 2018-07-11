#include "SkillsWidget.h"

#include <QFrame>
#include <QGridLayout>
#include <QtWidgets/QLabel>

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

      QVBoxLayout* layoutContent = new QVBoxLayout();
      layoutContent->addWidget(label);

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