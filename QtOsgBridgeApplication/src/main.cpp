#include "QtOsgBridgeApplication.h"
#include "LoadingInitialState.h"

int main(int argc, char** argv)
{
  /*auto format = QSurfaceFormat::defaultFormat();
  format.setColorSpace(QSurfaceFormat::ColorSpace::sRGBColorSpace);
  QSurfaceFormat::setDefaultFormat(format);*/

  QtOsgBridgeApplication app(argc, argv);
  return app.runGame<LoadingInitialState>();
}
