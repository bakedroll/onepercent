#include "DebugWindow.h"

#include "core/QConnectFunctor.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <chrono>

namespace onep
{
  struct DebugWindow::Impl
  {
    Impl(DebugWindow* b)
      : base(b)
      , toggleCountryButton(nullptr)
    {
      
    }

    DebugWindow* base;

    QPushButton* toggleCountryButton;

    osg::ref_ptr<GlobeOverviewWorld> world;
    CountryMesh::Map selectedCountries;

    osgGaming::Observer<int>::Ptr notifySelectedCountry;

    void toggleCountry()
    {
      auto start_time = std::chrono::high_resolution_clock::now();

      CountryMesh::Ptr mesh = world->getGlobeModel()->getCountryOverlay()->getSelectedCountryMesh();
      if (!mesh.valid())
        return;

      CountryMesh::Map::iterator it = selectedCountries.find(mesh->getCountryData()->getId());
      if (it == selectedCountries.end())
        selectedCountries[mesh->getCountryData()->getId()] = mesh;
      else
        selectedCountries.erase(it);

      CountryMesh::List meshlist;
      for (CountryMesh::Map::iterator cit = selectedCountries.begin(); cit != selectedCountries.end(); ++cit)
        meshlist.push_back(cit->second);

      world->getGlobeModel()->getBoundariesMesh()->makeCountryBoundaries(meshlist, osg::Vec3f(1.0f, 0.5, 0.1f), 0.009f);

      auto duration = std::chrono::high_resolution_clock::now() - start_time;

      long d = std::chrono::duration_cast<std::chrono::milliseconds> (duration).count();
      printf("Took %d ms\n", d);
    }

    void setupUi()
    {
      toggleCountryButton = new QPushButton("Toggle selected country");

      QConnectFunctor::connect(toggleCountryButton, SIGNAL(clicked()), [this]()
      {
        toggleCountry();
      });

      QVBoxLayout* layout = new QVBoxLayout();
      layout->addWidget(toggleCountryButton);
      layout->addStretch(1);

      base->setLayout(layout);
    }
  };

  DebugWindow::DebugWindow(osg::ref_ptr<GlobeOverviewWorld> world, QWidget* parent)
    : QDialog(parent)
    , m(new Impl(this))
  {
    setMinimumSize(QSize(200, 300));

    m->world = world;

    m->setupUi();

    m->notifySelectedCountry = m->world->getGlobeModel()->getCountryOverlay()->getSelectedCountryIdObservable()->connectAndNotify(osgGaming::Func<int>([this](int selected)
    {
      m->toggleCountryButton->setEnabled(selected > 0);
    }));
  }

  DebugWindow::~DebugWindow()
  {
  }
}