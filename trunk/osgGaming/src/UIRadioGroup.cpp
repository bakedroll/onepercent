#include <osgGaming/UIRadioGroup.h>

namespace osgGaming
{
  UIRadioGroup::UIRadioGroup()
    : osg::Referenced()
    , m_selected(new Observable<UIButton::Ptr>(nullptr))
    , m_resetting(false)
  {
  }

  void UIRadioGroup::addButton(UIButton::Ptr button)
  {
    button->setCheckable(true);
    m_buttons.push_back(button);

    m_isCheckedObserver = button->getIsCheckedObservable()->connect([this, button](bool checked)
    {
      if (m_resetting)
        return;

      if (checked)
      {
        for (ButtonList::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it)
        {
          if (*it != button)
          {
            (*it)->setChecked(false);
          }
        }

        m_selected->set(button);
      }
      else
      {
        m_selected->set(nullptr);
      }
    });
  }

  UIButton::Ptr UIRadioGroup::getSelectedButton()
  {
    return m_selected->get();
  }

  Observable<UIButton::Ptr>::Ptr UIRadioGroup::getSelectedButtonObservable()
  {
    return m_selected;
  }

  void UIRadioGroup::reset()
  {
    if (!m_selected->get())
      return;

    m_resetting = true;
    m_selected->get()->setChecked(false);
    m_resetting = false;
  }
}