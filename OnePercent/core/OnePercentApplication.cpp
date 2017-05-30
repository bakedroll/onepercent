#include "OnePercentApplication.h"

#include "widgets/MainWindow.h"
#include "nodes/GlobeOverviewWorld.h"
#include "states/GlobeOverviewState.h"
#include "states/MainMenuState.h"
#include "states/LoadingGlobeOverviewState.h"
#include "osgGaming/ResourceManager.h"
#include "osgGaming/Observable.h"

namespace onep
{
  struct OnePercentApplication::Impl
  {
    MainWindow* mainWindow;

    osgGaming::Observer<void>::Ptr endGameObserver;
  };

  OnePercentApplication::OnePercentApplication(int argc, char** argv)
    : QtGameApplication(argc, argv)
    , m(new Impl())
  {
    initialize();
  }

  void OnePercentApplication::initialize()
  {
    osgGaming::ResourceManager::getInstance()->setDefaultFontResourceKey("./GameData/fonts/coolvetica rg.ttf");

    getViewer()->addView(new osgGaming::View());

    m->mainWindow = new MainWindow(getViewer());
    m->mainWindow->show();
    setInputManager(m->mainWindow->getViewWidget());

    setDefaultWorld(new GlobeOverviewWorld());

    m->endGameObserver = onEndGameSignal().connect(osgGaming::Func<void>([this]()
    {
      m->mainWindow->shutdown();
    }));
  }

  int OnePercentApplication::run()
  {
    osgGaming::GameState::AbstractGameStateList states;
    states.push_back(new GlobeOverviewState());
    states.push_back(new MainMenuState());

    return GameApplication::run(new LoadingGlobeOverviewState(states));
  }
}