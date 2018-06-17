#include "osgGaming/NativeGameApplication.h"

#include <osgGaming/GameSettings.h>
#include <osgGaming/NativeInputManager.h>
#include <osgGaming/View.h>

#include <assert.h>

namespace osgGaming
{
  NativeGameApplication::NativeGameApplication(osg::ref_ptr<View> view)
    : GameApplication()
  {
    if (!view)
      view = new osgGaming::View();

    getViewer()->addView(view);
  }

  int NativeGameApplication::mainloop()
  {
    osg::ref_ptr<GameSettings> settings = getDefaultGameSettings();

    unsigned int nviews = getViewer()->getNumViews();
    for (unsigned int i = 0; i < nviews; i++)
    {
      getViewer()->setFullscreenEnabled(i, settings->getFullscreenEnabled());
      getViewer()->setWindowedResolution(i, settings->getWindowedResolution());

      View* view = getViewer()->getView(i);

      if (!view)
      {
        assert(false);
        return -1;
      }

      view->setScreenNum(settings->getScreenNum());
      view->setupResolution();
    }

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