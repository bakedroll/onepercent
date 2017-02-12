#include "osgGaming/NativeGameApplication.h"

#include <osgGaming/GameSettings.h>
#include <osgGaming/NativeInputManager.h>
#include <osgGaming/NativeView.h>

#include <assert.h>

namespace osgGaming
{
  NativeGameApplication::NativeGameApplication(osg::ref_ptr<View> view)
    : GameApplication()
  {
    if (!view)
      view = new osgGaming::NativeView();

    getViewer()->addView(view);
  }

  int NativeGameApplication::mainloop()
  {
    osg::ref_ptr<GameSettings> settings = getDefaultGameSettings();

    
    getViewer()->setFullscreenEnabled(0, settings->getFullscreenEnabled());
    getViewer()->setWindowedResolution(0, settings->getWindowedResolution());

    View* view = dynamic_cast<View*>(getViewer()->getView(0));

    if (!view)
    {
      assert(false);
      return -1;
    }

    view->setScreenNum(settings->getScreenNum());
    view->setupResolution();

    getViewer()->setKeyEventSetsDone(0);
    getViewer()->realize();

    while (!getViewer()->done() && !isGameEnded())
    {
      getViewer()->frame();
    }

    return 0;
  }

  osg::ref_ptr<InputManager> NativeGameApplication::createInputManager(osg::ref_ptr<osgGaming::View> view)
  {
    NativeInputManager* im = new NativeInputManager();
    view->addEventHandler(im->handler());
    return im;
  }

}