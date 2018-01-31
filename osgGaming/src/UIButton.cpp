#include <osgGaming/UIButton.h>
#include <osgGaming/Helper.h>

#include <osg/Geode>
#include <osg/Geometry>

namespace osgGaming
{

  UIButton::UIButton()
    : UIVisualElement()
    , UserInteractionModel()
    , m_text("")
    , m_fontSize(18.0)
    , m_checkable(false)
    , m_isChecked(new Observable<bool>(false))
  {
    setPadding(6.0f);

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    osg::ref_ptr<osg::Geometry> geo = createQuadGeometry(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, XY, true); 

    geode->addDrawable(geo);

    m_material = new osg::Material();
    m_material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    m_material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    m_material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    m_material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.5f, 0.5f, 1.0f));
    m_material->setTransparency(osg::Material::FRONT_AND_BACK, 0.5f);
    geode->getOrCreateStateSet()->setAttributeAndModes(m_material, osg::StateAttribute::ON);

    getVisualGroup()->addChild(geode);

    osg::ref_ptr<osg::Geode> textGeode = new osg::Geode();

    m_textNode = createTextNode(m_text, m_fontSize);
    m_textNode->setAlignment(osgText::TextBase::CENTER_CENTER);

    MatrixTransform::addChild(textGeode);
    textGeode->addDrawable(m_textNode);
  }

  void UIButton::setFontSize(float fontSize)
  {
    m_fontSize = fontSize;
    m_textNode->setCharacterSize(fontSize);
  }

  void UIButton::setText(std::string text)
  {
    m_text = text;
    m_textNode->setText(text);
  }

  void UIButton::onMouseEnter()
  {
    if (isEnabled() && !isCheckable())
      m_material->setTransparency(osg::Material::FRONT_AND_BACK, 0.0f);
  }

  void UIButton::onMouseLeave()
  {
    if (!m_isChecked->get())
      m_material->setTransparency(osg::Material::FRONT_AND_BACK, 0.5f);
  }

  void UIButton::onClicked()
  {
    if (m_checkable)
      setChecked(!m_isChecked->get());
  }

  void UIButton::onEnabledChanged(bool enabled)
  {
    if (enabled)
    {
      m_textNode->setColor(osg::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    }
    else
    {
      m_textNode->setColor(osg::Vec4f(0.6f, 0.6f, 0.6f, 1.0f));
    }
  }

  void UIButton::getAbsoluteOriginSize(osg::Vec2f& origin, osg::Vec2f& size)
  {
    osg::Vec4f margin = getMargin();

    origin = getAbsoluteOrigin();
    size = getSize() - osg::Vec2f(margin.x() + margin.z(), margin.y() + margin.w());
  }

  void UIButton::updatedContentOriginSize(osg::Vec2f origin, osg::Vec2f size)
  {
    osg::Vec2f position = origin + size / 2.0f;
    m_textNode->setPosition(osg::Vec3f(position.x(), position.y(), 0.0f));
  }

  bool UIButton::isCheckable()
  {
    return m_checkable;
  }

  bool UIButton::isChecked()
  {
    return m_isChecked->get();
  }

  Observable<bool>::Ptr UIButton::getIsCheckedObservable()
  {
    return m_isChecked;
  }

  void UIButton::setCheckable(bool checkable)
  {
    m_checkable = checkable;
    setChecked(false);
  }

  void UIButton::setChecked(bool checked)
  {
    if (!m_checkable && getHovered())
      m_material->setTransparency(osg::Material::FRONT_AND_BACK, 0.0f);

    if (!m_checkable || m_isChecked->get() == checked)
      return;

    m_isChecked->set(checked);
    if (checked)
      m_material->setTransparency(osg::Material::FRONT_AND_BACK, 0.0f);
    else
      m_material->setTransparency(osg::Material::FRONT_AND_BACK, 0.5f);
  }

  osg::Vec2f UIButton::calculateMinContentSize()
  {
    return getTextSize(m_textNode);
  }

}