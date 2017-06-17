#include <osgGaming/Hud.h>
#include <osgGaming/FpsTextCallback.h>
#include <osgGaming/UIUpdateVisitor.h>
#include <osgGaming/UIMCollectorVisitor.h>
#include <osgGaming/UIFindElementVisitor.h>
#include <osgGaming/UIMarkupLoader.h>
#include <osgGaming/UIElement.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/Helper.h>

#include <osg/Geode>

#include <osgText/Text>

namespace osgGaming
{

  struct Hud::Impl
  {
    Impl()
      : fpsEnabled(false)
    {}

    osg::ref_ptr<osg::Projection> projection;
    osg::ref_ptr<osg::Geode> geode;

    osg::ref_ptr<UIElement> rootUIElement;
    osg::ref_ptr<osg::MatrixTransform> modelViewTransform;

    osg::ref_ptr<osgText::Text> fpsText;
    bool fpsEnabled;

    osg::Vec2f resolution;

    UserInteractionModel::List uimList;
  };

  Hud::Hud()
    : Referenced()
    , m(new Impl())
  {
    osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();
    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    stateSet->setRenderBinDetails(10, "RenderBin");


    m->geode = new osg::Geode();

    m->rootUIElement = new UIGrid();

    m->modelViewTransform = new osg::MatrixTransform();
    m->modelViewTransform->setMatrix(osg::Matrix::identity());
    m->modelViewTransform->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    m->modelViewTransform->addChild(m->geode);
    m->modelViewTransform->addChild(m->rootUIElement);

    m->projection = new osg::Projection();
    m->projection->addChild(m->modelViewTransform);
    m->projection->setStateSet(stateSet);
  }

  Hud::~Hud()
  {
  }

  osg::ref_ptr<osg::Projection> Hud::getProjection()
  {
    return m->projection;
  }

  osg::ref_ptr<osg::MatrixTransform> Hud::getModelViewTransform()
  {
    return m->modelViewTransform;
  }

  void Hud::loadMarkupFromXmlResource(std::string resourceKey)
  {
    osg::ref_ptr<UIElement> element = UIMarkupLoader::getInstance()->loadMarkupFromXmlResource(resourceKey);

    setRootUIElement(element);
  }

  osg::ref_ptr<UIElement> Hud::getRootUIElement()
  {
    return m->rootUIElement;
  }

  UserInteractionModel::List& Hud::getUserInteractionModels()
  {
    return m->uimList;
  }

  osg::ref_ptr<UIElement> Hud::getUIElementByName(std::string name)
  {
    UIFindElementVisitor visitor(name);

    m->rootUIElement->accept(visitor);

    return visitor.getResult();
  }

  void Hud::updateResolution(osg::Vec2f resolution)
  {
    m->resolution = resolution;

    m->projection->setMatrix(osg::Matrix::ortho2D(0.0, double(m->resolution.x()) - 1.0, 0.0, double(m->resolution.y()) - 1.0));

    updateUIElements();
  }

  void Hud::updateUIElements()
  {
    osg::Vec2f origin(0.0f, 0.0f);

    m->rootUIElement->setAbsoluteOrigin(origin);
    m->rootUIElement->setOrigin(origin);
    m->rootUIElement->setSize(m->resolution);

    UIUpdateVisitor uiUpdateVisitor;
    m->rootUIElement->accept(uiUpdateVisitor);
  }

  void Hud::setFpsEnabled(bool enabled)
  {
    if (enabled == m->fpsEnabled)
    {
      return;
    }

    m->fpsEnabled = enabled;

    if (!m->fpsText.valid())
    {
      m->fpsText = createTextNode("", 25.0f, ResourceManager::getInstance()->loadDefaultFont());
      m->fpsText->setAlignment(osgText::TextBase::LEFT_BOTTOM);
      m->fpsText->setPosition(osg::Vec3f(10.0f, 10.0f, 0.0f));
      m->fpsText->setDataVariance(osg::Object::DYNAMIC);
      m->fpsText->setUpdateCallback(new FpsTextCallback());
    }

    if (m->fpsEnabled == true)
    {
      m->geode->addDrawable(m->fpsText);
    }
    else
    {
      m->geode->removeDrawable(m->fpsText);
    }
  }

  void Hud::setRootUIElement(osg::ref_ptr<UIElement> element)
  {
    if (m->rootUIElement == element)
    {
      return;
    }

    osg::ref_ptr<osg::Group> parent = m->rootUIElement->getParent(0);
    parent->removeChild(m->rootUIElement);

    m->rootUIElement = element;

    parent->addChild(m->rootUIElement);

    resetUserInteractionModel();
    m->uimList.clear();

    UIMCollectorVisitor visitor;
    m->rootUIElement->accept(visitor);

    m->uimList = visitor.getUserInteractionModels();

    updateUIElements();
  }

  void Hud::registerUserInteractionModel(UserInteractionModel* model)
  {
    m->uimList.push_back(model);
  }

  bool Hud::anyUserInteractionModelHovered()
  {
    for (UserInteractionModel::List::iterator it = m->uimList.begin(); it != m->uimList.end(); ++it)
    {
      if ((*it)->getHovered())
      {
        return true;
      }
    }

    return false;
  }

  void Hud::resetUserInteractionModel()
  {
    for (UserInteractionModel::List::iterator it = m->uimList.begin(); it != m->uimList.end(); ++it)
    {
      (*it)->setHovered(false);
    }
  }

}
