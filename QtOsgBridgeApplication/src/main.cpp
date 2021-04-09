#include "QtOsgBridgeApplication.h"
#include "LoadingInitialState.h"

int main(int argc, char** argv)
{
  auto format = QSurfaceFormat::defaultFormat();
  format.setColorSpace(QSurfaceFormat::ColorSpace::sRGBColorSpace);
  format.setBlueBufferSize(16);
  format.setRedBufferSize(16);
  format.setGreenBufferSize(16);
  format.setOption(QSurfaceFormat::FormatOption::StereoBuffers);
  format.setVersion(4, 1);
  format.setProfile(QSurfaceFormat::OpenGLContextProfile::CompatibilityProfile);
  QSurfaceFormat::setDefaultFormat(format);

  QtOsgBridgeApplication app(argc, argv);
  return app.runGame<LoadingInitialState>();
}
