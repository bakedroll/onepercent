#pragma once

#include <osg/Referenced>
#include <osgGaming/UIButton.h>

namespace osgGaming
{
  class UIRadioGroup : public osg::Referenced
  {
  public:
    UIRadioGroup();

    typedef osg::ref_ptr<UIRadioGroup> Ptr;

    void addButton(UIButton::Ptr button);

    UIButton::Ptr getSelectedButton();
    Observable<UIButton::Ptr>::Ptr getSelectedButtonObservable();

    void reset();

  private:
    typedef std::vector<UIButton::Ptr> ButtonList;

    ButtonList m_buttons;
    Observable<UIButton::Ptr>::Ptr m_selected;

    bool  m_resetting;

    Observer<bool>::Ptr m_isCheckedObserver;
  };
}