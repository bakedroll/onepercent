#include "OsgWidget.h"

#include <assert.h>

#include <osgGaming/View.h>
#include <QTimer>
#include <QResizeEvent>

namespace onep
{
  typedef std::map<int, int> IntMap;

  IntMap makeButtonsMap()
  {
    IntMap map;
    map[Qt::LeftButton] = osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON;
    map[Qt::MiddleButton] = osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON;
    map[Qt::RightButton] = osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON;
    return map;
  }

  IntMap makeKeysMap()
  {
// TODO: Complete this map

    IntMap map;
    map[Qt::Key_Escape] = osgGA::GUIEventAdapter::KEY_Escape;
    map[Qt::Key_Tab] = osgGA::GUIEventAdapter::KEY_Tab;
    map[Qt::Key_Backtab] = 0;//osgGA::GUIEventAdapter::Key_Back;
    map[Qt::Key_Backspace] = osgGA::GUIEventAdapter::KEY_BackSpace;
    map[Qt::Key_Return] = osgGA::GUIEventAdapter::KEY_Return;
    map[Qt::Key_Enter] = osgGA::GUIEventAdapter::KEY_KP_Enter;
    map[Qt::Key_Insert] = osgGA::GUIEventAdapter::KEY_Insert;
    map[Qt::Key_Delete] = osgGA::GUIEventAdapter::KEY_Delete;
    map[Qt::Key_Pause] = osgGA::GUIEventAdapter::KEY_Pause;
    map[Qt::Key_Print] = osgGA::GUIEventAdapter::KEY_Print;
    map[Qt::Key_SysReq] = osgGA::GUIEventAdapter::KEY_Sys_Req;
    map[Qt::Key_Clear] = osgGA::GUIEventAdapter::KEY_Clear;
    map[Qt::Key_Home] = osgGA::GUIEventAdapter::KEY_Home;
    map[Qt::Key_End] = osgGA::GUIEventAdapter::KEY_End;
    map[Qt::Key_Left] = osgGA::GUIEventAdapter::KEY_Left;
    map[Qt::Key_Up] = osgGA::GUIEventAdapter::KEY_Up;
    map[Qt::Key_Right] = osgGA::GUIEventAdapter::KEY_Right;
    map[Qt::Key_Down] = osgGA::GUIEventAdapter::KEY_Down;
    map[Qt::Key_PageUp] = osgGA::GUIEventAdapter::KEY_Page_Up;
    map[Qt::Key_PageDown] = osgGA::GUIEventAdapter::KEY_Page_Down;
    map[Qt::Key_Shift] = osgGA::GUIEventAdapter::KEY_Shift_L;
    map[Qt::Key_Control] = osgGA::GUIEventAdapter::KEY_Control_L;
    map[Qt::Key_Meta] = osgGA::GUIEventAdapter::KEY_Meta_L;
    map[Qt::Key_Alt] = osgGA::GUIEventAdapter::KEY_Alt_L;
    map[Qt::Key_AltGr] = osgGA::GUIEventAdapter::KEY_Alt_R;
    map[Qt::Key_CapsLock] = osgGA::GUIEventAdapter::KEY_Caps_Lock;
    map[Qt::Key_NumLock] = osgGA::GUIEventAdapter::KEY_Num_Lock;
    map[Qt::Key_ScrollLock] = osgGA::GUIEventAdapter::KEY_Scroll_Lock;
    map[Qt::Key_Super_L] = osgGA::GUIEventAdapter::KEY_Super_L;
    map[Qt::Key_Super_R] = osgGA::GUIEventAdapter::KEY_Super_R;
    map[Qt::Key_Menu] = osgGA::GUIEventAdapter::KEY_Menu;
    map[Qt::Key_Hyper_L] = osgGA::GUIEventAdapter::KEY_Hyper_L;
    map[Qt::Key_Hyper_R] = osgGA::GUIEventAdapter::KEY_Hyper_R;
    map[Qt::Key_Help] = osgGA::GUIEventAdapter::KEY_Help;
    map[Qt::Key_Direction_L] = 0; //osgGA::GUIEventAdapter::KEY_Leftparen??;
    map[Qt::Key_Direction_R] = 0; //osgGA::GUIEventAdapter::KEY_Tab;
    map[Qt::Key_Space] = osgGA::GUIEventAdapter::KEY_Space;
    map[Qt::Key_Any] = osgGA::GUIEventAdapter::KEY_Space;
    map[Qt::Key_Exclam] = osgGA::GUIEventAdapter::KEY_Exclaim;
    map[Qt::Key_QuoteDbl] = osgGA::GUIEventAdapter::KEY_Quotedbl;
    map[Qt::Key_NumberSign] = osgGA::GUIEventAdapter::KEY_Hash;
    map[Qt::Key_Dollar] = osgGA::GUIEventAdapter::KEY_Dollar;
    map[Qt::Key_Percent] = // osgGA::GUIEventAdapter::KEY_p;
    map[Qt::Key_Ampersand] = osgGA::GUIEventAdapter::KEY_Ampersand;
    map[Qt::Key_ParenLeft] = osgGA::GUIEventAdapter::KEY_Leftparen;
    map[Qt::Key_ParenRight] = osgGA::GUIEventAdapter::KEY_Rightparen;
    map[Qt::Key_Asterisk] = osgGA::GUIEventAdapter::KEY_Asterisk;
    map[Qt::Key_Plus] = osgGA::GUIEventAdapter::KEY_Plus;
    map[Qt::Key_Comma] = osgGA::GUIEventAdapter::KEY_Comma;
    map[Qt::Key_Minus] = osgGA::GUIEventAdapter::KEY_Minus;
    map[Qt::Key_Period] = osgGA::GUIEventAdapter::KEY_Period;
    map[Qt::Key_Slash] = osgGA::GUIEventAdapter::KEY_Slash;
    map[Qt::Key_BracketLeft] = osgGA::GUIEventAdapter::KEY_Leftbracket;
    map[Qt::Key_Backslash] = osgGA::GUIEventAdapter::KEY_Backslash;
    map[Qt::Key_BracketRight] = osgGA::GUIEventAdapter::KEY_Rightbracket;
    map[Qt::Key_AsciiCircum] = 0;//osgGA::GUIEventAdapter::KEY_;
    map[Qt::Key_Underscore] = osgGA::GUIEventAdapter::KEY_Underscore;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_BraceLeft] = 0;//osgGA::GUIEventAdapter::KEY_Leftbracket;
    map[Qt::Key_Bar] = 0;//osgGA::GUIEventAdapter::KEY_Bar;
    map[Qt::Key_BraceRight] = 0;// osgGA::GUIEventAdapter::KEY_Rightbracket;
    map[Qt::Key_AsciiTilde] = 0;// osgGA::GUIEventAdapter::KEY_;


    map[Qt::Key_Less] = osgGA::GUIEventAdapter::KEY_Less;
    map[Qt::Key_Equal] = osgGA::GUIEventAdapter::KEY_Equals;
    map[Qt::Key_Greater] = osgGA::GUIEventAdapter::KEY_Greater;
    map[Qt::Key_Question] = osgGA::GUIEventAdapter::KEY_Question;
    //map[Qt::Key_Caret] = osgGA::GUIEventAdapter::KEY_Caret;
    map[Qt::Key_Apostrophe] = osgGA::GUIEventAdapter::KEY_Backquote;
    //map[Qt::Key_f] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;
    map[Qt::Key_QuoteLeft] = osgGA::GUIEventAdapter::KEY_Quote;


    for (int i = 0; i < 25; i++)
      map[Qt::Key_F1 + i] = osgGA::GUIEventAdapter::KEY_F1 + i;

    for (int i = 0; i < 10; i++)
      map[Qt::Key_0 + i] = osgGA::GUIEventAdapter::KEY_0 + i;

    for (int i = 0; i < 26; i++)
      map[Qt::Key_A + i] = osgGA::GUIEventAdapter::KEY_A + i;

    for (int i = 0; i < 7; i++)
      map[Qt::Key_Colon + i] = osgGA::GUIEventAdapter::KEY_Colon + i;

    return map;
  }

  static IntMap c_buttons = makeButtonsMap();
  static IntMap c_keys = makeKeysMap();

  class EventAdapter : public osgGA::GUIEventAdapter
  {
  public:
    EventAdapter() : osgGA::GUIEventAdapter() {}
    EventAdapter(QMouseEvent* event, int height)
      : osgGA::GUIEventAdapter()
    {
      setX(event->x());
      setY(height - event->y() - 1);
      setButton(c_buttons[event->button()]);
    }
    EventAdapter(QWheelEvent* event)
      : osgGA::GUIEventAdapter()
    {
      setScrollingMotion(event->delta() < 0 ? SCROLL_DOWN : SCROLL_UP);
    }
    EventAdapter(QKeyEvent* event)
      : osgGA::GUIEventAdapter()
    {
      setKey(c_keys[event->key()]);
    }
  };

  struct OsgWidget::Impl
  {
    Impl(osg::ref_ptr<osgViewer::CompositeViewer> viewer)
      : viewer(viewer)
      , isInitialized(false)
    {
    }

    osg::ref_ptr<osgViewer::CompositeViewer> viewer;
    osg::ref_ptr<osgGaming::View> view;
    osg::ref_ptr<osg::Camera> camera;
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow;

    QTimer updateTimer;

    bool isInitialized;
  };

  OsgWidget::OsgWidget(osg::ref_ptr<osgViewer::CompositeViewer> viewer, QWidget* parent, Qt::WindowFlags f)
    : QGLWidget(parent, nullptr, f)
    , InputManager()
    , m(new Impl(viewer))
  {
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    //setWindowFlags(Qt::FramelessWindowHint);
    //setAttribute(Qt::WA_NoSystemBackground);
    //setAttribute(Qt::WA_OpaquePaintEvent);
    //setAttribute(Qt::WA_TranslucentBackground);

    m->graphicsWindow = new osgViewer::GraphicsWindowEmbedded(x(), y(), width(), height());
    m->view = dynamic_cast<osgGaming::View*>(viewer->getView(0));
    m->camera = m->view->getSceneCamera();

    if (!m->camera || !m->view)
    {
      assert(false && "Please set camera");
      return;
    }

    osg::ref_ptr<osg::Camera> hudcam = m->view->getHudCamera();

    m->camera->setGraphicsContext(m->graphicsWindow);
    hudcam->setGraphicsContext(m->graphicsWindow);

    m->camera->setViewport(0, 0, width(), height());
    hudcam->setViewport(0, 0, width(), height());

    m->updateTimer.setInterval(16);
    m->updateTimer.setSingleShot(false);
    
    connect(&m->updateTimer, SIGNAL(timeout()), this, SLOT(update()));

    m->updateTimer.start();
  }

  OsgWidget::~OsgWidget()
  {
  }

  void OsgWidget::paintGL()
  {
    m->viewer->frame();
  }

  void OsgWidget::resizeGL(int width, int height)
  {    
    if (isInitialized())
    {
      EventAdapter ea;
      ea.setWindowX(geometry().x());
      ea.setWindowY(geometry().y());
      ea.setWindowWidth(width);
      ea.setWindowHeight(height);
      onResizeEvent(ea);
    }
    else
    {
      m->view->updateResolution(osg::Vec2f(width, height));
    }

    m->graphicsWindow->getEventQueue()->windowResize(x(), y(), width, height);
    m->graphicsWindow->resized(x(), y(), width, height);

    // dirty workaround to fit the render quad
    float factor = float(width) / float(height) * (3.0f / 4.0f);

    osg::ref_ptr<osg::Geode> geode = m->view->getCanvasGeode();
    geode->removeDrawable(geode->getDrawable(0));
    geode->addDrawable(osg::createTexturedQuadGeometry(osg::Vec3f(-factor, -1.0f, 0.0f), osg::Vec3f(2.0f * factor, 0.0f, 0.0f), osg::Vec3f(0.0f, 2.0f, 0.0f)));
  }

  void OsgWidget::keyPressEvent(QKeyEvent* event)
  {
    EventAdapter ea(event);
    onKeyPressEvent(ea);
  }

  void OsgWidget::keyReleaseEvent(QKeyEvent* event)
  {
    EventAdapter ea(event);
    onKeyReleaseEvent(ea);
  }

  void OsgWidget::mouseMoveEvent(QMouseEvent* event)
  {
    EventAdapter ea(event, height());
    onMouseMoveEvent(ea);
  }

  void OsgWidget::mousePressEvent(QMouseEvent* event)
  {
    EventAdapter ea(event, height());
    onMouseClickEvent(ea);
  }

  void OsgWidget::mouseReleaseEvent(QMouseEvent* event)
  {
    EventAdapter ea(event, height());
    onMouseReleaseEvent(ea);
  }

  void OsgWidget::wheelEvent(QWheelEvent* event)
  {
    EventAdapter ea(event);
    onMouseScrollEvent(ea);
  }
}