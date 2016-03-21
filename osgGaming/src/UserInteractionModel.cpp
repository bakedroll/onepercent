#include <osgGaming/UserInteractionModel.h>

namespace osgGaming
{

  UserInteractionModel::~UserInteractionModel()
  {
  }

  UserInteractionModel::UserInteractionModel()
    : m_hovered(false)
    , m_enabled(true)
  {

  }

  void UserInteractionModel::onClicked()
  {
  }

  void UserInteractionModel::onEnabledChanged(bool enabled)
  {

  }

  bool UserInteractionModel::isEnabled()
  {
    return m_enabled;
  }

  bool UserInteractionModel::getHovered()
  {
    return m_hovered;
  }

  void UserInteractionModel::setEnabled(bool enabled)
  {
    m_enabled = enabled;
    onEnabledChanged(enabled);
  }

  void UserInteractionModel::setHovered(bool hovered)
  {
    m_hovered = hovered;
  }

}