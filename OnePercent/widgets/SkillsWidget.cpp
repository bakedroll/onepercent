#include "SkillsWidget.h"

namespace onep
{
  struct SkillsWidget::Impl
  {
    Impl(osgGaming::Injector& injector) {}
  };

  SkillsWidget::SkillsWidget(osgGaming::Injector& injector)
    : VirtualOverlay()
    , m(new Impl(injector))
  {
  }

  SkillsWidget::~SkillsWidget()
  {
  }
}